#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include "lz77.h"
#include "bitbuffer.h"
#include "memory_slice.h"
#include "utilities.h"
#include "debug.h"

#define LZ77_SEARCH_BUFFER_SIZE 4096
#define LZ77_LOOKAHEAD_BUFFER_SIZE 32

#define LZ77_TOKEN_TYPE_SIZE 1
#define LZ77_TOKEN_POSITION_SIZE 12
#define LZ77_TOKEN_LENGTH_SIZE 6
#define LZ77_TOKEN_NEXT_SIZE 8

#define LZ77_TOKEN_TYPE_SYMBOL 0lu
#define LZ77_TOKEN_TYPE_PHRASE 1lu

#define PACK8(var, target_size) var = var << (8 - target_size);
#define UNPACK8(var, target_size) var = var >> (8 - target_size);
#define PACK32(var, target_size) var = htonl(var << (32 - target_size));
#define UNPACK32(var, target_size) var = ntohl(var) >> (32 - target_size);

static void find_longest_phrase(struct memory_slice *search_buffer,
								struct memory_slice *lookahead_buffer,
								unsigned int *phrase_position,
								unsigned int *phrase_length,
								unsigned char *next_symbol)
{
	const unsigned char *search_begin = memory_slice_begin(search_buffer);
	size_t search_limit = (const unsigned char *)memory_slice_end(search_buffer) - search_begin;

	const unsigned char *lookahead_begin = memory_slice_begin(lookahead_buffer);
	size_t lookahead_limit = (const unsigned char *)memory_slice_end(lookahead_buffer) - lookahead_begin;

	*phrase_position = 0;
	*phrase_length = 0;
	*next_symbol = lookahead_begin[0];

	int longest_phrase_length = 0;
	for (int search_index = 0; search_index < search_limit; search_index++)
	{
		int test_phrase_length = 0;
		while (1)
		{
			if (search_index + test_phrase_length >= search_limit)
				break;
			if (test_phrase_length >= lookahead_limit)
				break;
			if (search_begin[search_index + test_phrase_length] != lookahead_begin[test_phrase_length])
				break;

			test_phrase_length++;
		}

		if (test_phrase_length > longest_phrase_length)
		{
			longest_phrase_length = test_phrase_length;

			*phrase_position = search_index;
			*phrase_length = test_phrase_length;
			*next_symbol = longest_phrase_length < memory_slice_data_left(lookahead_buffer) ? lookahead_begin[longest_phrase_length] : '\0';
		}

		// there is no need to inspect end of search buffer if it is less than the longest phrase
		if (search_index + longest_phrase_length >= search_limit)
			break;
	}
}

int lz77_compress(unsigned char **compressed_data_raw, size_t *compressed_data_bits_count, const unsigned char *data, size_t data_bytes_count)
{
	DLOG("COMPRESSION")

	// INITIALIZE ALL BUFFERS

	struct bitbuffer_w compressed_data;
	RET_IF_NZ(bitbuffer_w_initialize(&compressed_data, data_bytes_count / 2))

	struct memory_slice search_buffer;
	memory_slice_initialize(&search_buffer, LZ77_SEARCH_BUFFER_SIZE, data, data_bytes_count);
	memory_slice_shift_left(&search_buffer, LZ77_SEARCH_BUFFER_SIZE);

	struct memory_slice lookahead_buffer;
	memory_slice_initialize(&lookahead_buffer, LZ77_LOOKAHEAD_BUFFER_SIZE, data, data_bytes_count);

	// WRITE HEADER

	// we will store compressed data size here
	RET_IF_NZ(bitbuffer_w_skip(&compressed_data, sizeof(size_t) * 8))

	// store original data size
	RET_IF_NZ(bitbuffer_w_write(&compressed_data, &data_bytes_count, sizeof(size_t) * 8))

	// WRITE COMPRESSED DATA

	while (memory_slice_data_left(&lookahead_buffer) > 0)
	{
		// FIND NEXT PHRASE

		unsigned int phrase_position;
		unsigned int phrase_length;
		unsigned char next_symbol;
		find_longest_phrase(&search_buffer, &lookahead_buffer, &phrase_position, &phrase_length, &next_symbol);

		// SERIALIZE THE PHRASE INTO A TOKEN

		unsigned int windows_shift;
		if (phrase_length == 0)
		{
			DLOG("SERIALIZE SYMBOL: %c", next_symbol)

			windows_shift = 1;

			unsigned char type = LZ77_TOKEN_TYPE_SYMBOL;
			PACK32(type, LZ77_TOKEN_TYPE_SIZE);
			RET_IF_NZ(bitbuffer_w_write(&compressed_data, &type, 1))
			D(bitbuffer_w_dump(&compressed_data))

			RET_IF_NZ(bitbuffer_w_write(&compressed_data, &next_symbol, 8))
			D(bitbuffer_w_dump(&compressed_data))
		}
		else
		{
			DLOG("SERIALIZE PHRASE: %i, %i, %c", phrase_position, phrase_length, next_symbol)

			windows_shift = phrase_length + 1;

			unsigned char type = LZ77_TOKEN_TYPE_PHRASE;
			PACK8(type, LZ77_TOKEN_TYPE_SIZE);
			RET_IF_NZ(bitbuffer_w_write(&compressed_data, &type, 1))
			D(bitbuffer_w_dump(&compressed_data))

			PACK32(phrase_position, LZ77_TOKEN_POSITION_SIZE)
			RET_IF_NZ(bitbuffer_w_write(&compressed_data, &phrase_position, LZ77_TOKEN_POSITION_SIZE))
			D(bitbuffer_w_dump(&compressed_data))

			PACK32(phrase_length, LZ77_TOKEN_LENGTH_SIZE)
			RET_IF_NZ(bitbuffer_w_write(&compressed_data, &phrase_length, LZ77_TOKEN_LENGTH_SIZE))
			D(bitbuffer_w_dump(&compressed_data))

			RET_IF_NZ(bitbuffer_w_write(&compressed_data, &next_symbol, 8))
			D(bitbuffer_w_dump(&compressed_data))
		}

		// SHIFT WINDOWS

		memory_slice_shift_right(&search_buffer, windows_shift);
		memory_slice_shift_right(&lookahead_buffer, windows_shift);
	}

	// FINALIZE HEADER

	*((size_t *)compressed_data.data) = compressed_data.used_bits_count - 2 * sizeof(size_t) * 8;

	DLOG("FINAL HEADER: compressed_size=%lub, size=%luB", ((size_t *)compressed_data.data)[0], ((size_t *)compressed_data.data)[1])

	// WRITE RESULTS

	*compressed_data_raw = compressed_data.data;
	*compressed_data_bits_count = compressed_data.used_bits_count;

	return 0;
}

int lz77_decompress(unsigned char **data_ptr, const unsigned char *compressed_data_raw)
{
	DLOG("DECOMPRESSION")

	// READ HEADER

	const size_t compressed_data_bits_count = ((const size_t *)compressed_data_raw)[0];
	const size_t data_bytes_count = ((const size_t *)compressed_data_raw)[1];

	DLOG("HEADER: compressed_size=%lub, size=%luB", compressed_data_bits_count, data_bytes_count)

	// INITIALIZE BUFFERS AND SLICES

	struct bitbuffer_r compressed_data;
	bitbuffer_r_initialize(&compressed_data, compressed_data_raw + 2 * sizeof(size_t), compressed_data_bits_count);

	unsigned char *data = malloc(data_bytes_count + 1);
	data[data_bytes_count] = 0;
	D(memset(data, 0, data_bytes_count))

	*data_ptr = data;

	struct memory_slice search_buffer;
	memory_slice_initialize(&search_buffer, LZ77_SEARCH_BUFFER_SIZE, data, data_bytes_count);
	memory_slice_shift_left(&search_buffer, LZ77_SEARCH_BUFFER_SIZE);

	struct memory_slice lookahead_buffer;
	memory_slice_initialize(&lookahead_buffer, LZ77_LOOKAHEAD_BUFFER_SIZE, data, data_bytes_count);

	// UNCOMPRESS DATA

	while (bitbuffer_r_unread_count(&compressed_data) > 0)
	{
		D(bitbuffer_r_dump(&compressed_data))

		unsigned char *lookahead_ptr = (unsigned char *)memory_slice_begin(&lookahead_buffer);
		const unsigned char *search_ptr = memory_slice_begin(&search_buffer);

		unsigned char type = 0;
		RET_IF_NZ(bitbuffer_r_read(&compressed_data, &type, 1))
		UNPACK8(type, LZ77_TOKEN_TYPE_SIZE)

		unsigned int phrase_length = 0;
		if (type == LZ77_TOKEN_TYPE_SYMBOL)
		{
			unsigned char symbol = 0;
			RET_IF_NZ(bitbuffer_r_read(&compressed_data, &symbol, LZ77_TOKEN_NEXT_SIZE))

			*lookahead_ptr = symbol;

			DLOG("DESERIALIZE SYMBOL: %c", symbol)
		}
		else if (type == LZ77_TOKEN_TYPE_PHRASE)
		{
			unsigned int phrase_position = 0;
			RET_IF_NZ(bitbuffer_r_read(&compressed_data, &phrase_position, LZ77_TOKEN_POSITION_SIZE))
			UNPACK32(phrase_position, LZ77_TOKEN_POSITION_SIZE)

			D(bitbuffer_r_dump(&compressed_data))

			RET_IF_NZ(bitbuffer_r_read(&compressed_data, &phrase_length, LZ77_TOKEN_LENGTH_SIZE))
			UNPACK32(phrase_length, LZ77_TOKEN_LENGTH_SIZE)

			D(bitbuffer_r_dump(&compressed_data))

			unsigned char next_symbol = 0;
			RET_IF_NZ(bitbuffer_r_read(&compressed_data, &next_symbol, LZ77_TOKEN_NEXT_SIZE))

			D(bitbuffer_r_dump(&compressed_data))

			memcpy(lookahead_ptr, search_ptr + phrase_position, phrase_length);
			lookahead_ptr[phrase_length] = next_symbol;

			DLOG("DESERIALIZE PHRASE: %d, %d, %c", phrase_position, phrase_length, next_symbol)
		}
		else
		{
			return 1;
		}

		memory_slice_shift_right(&search_buffer, phrase_length + 1);
		memory_slice_shift_right(&lookahead_buffer, phrase_length + 1);

		D(printf("RESULT: %s\n", data))
	}

	return 0;
}