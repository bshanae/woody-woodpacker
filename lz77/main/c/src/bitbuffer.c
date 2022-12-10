#include "bitbuffer.h"
#include "utilities.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>

int bitbuffer_w_initialize(struct bitbuffer_w *buffer, size_t preallocated_bytes_count)
{
	buffer->data = malloc(preallocated_bytes_count);
	buffer->allocated_bytes_count = preallocated_bytes_count;
	buffer->used_bits_count = 0;

	return buffer->data == NULL;
}

void bitbuffer_w_uninitialize(struct bitbuffer_w *buffer)
{
	free(buffer->data);
}

static int bitbuffer_resize_if_needed(struct bitbuffer_w *buffer, size_t required_bytes_count)
{
	if ((float)buffer->allocated_bytes_count * 0.8f > (float)required_bytes_count)
		return 0;

	void *new_data = realloc(buffer->data, buffer->allocated_bytes_count * 2);
	if (new_data == NULL)
		return 1;

	buffer->data = new_data;
	buffer->allocated_bytes_count = required_bytes_count;

	return 0;
}

static int bitbuffer_w_write_(struct bitbuffer_w *buffer, const unsigned char *addendum, size_t addendum_bits)
{
	RET_IF_NZ(bitbuffer_resize_if_needed(buffer, CEIL(buffer->used_bits_count + addendum_bits, 8)))

	int free_bits_in_last_byte = INVERSE_MOD(buffer->used_bits_count, 8);
	const int addendum_bytes = CEIL(addendum_bits, 8);
	if (free_bits_in_last_byte > 0)
	{
		unsigned char *ptr = buffer->data + CEIL(buffer->used_bits_count, 8);

		// we should shift every byte on this value
		const unsigned char shift = free_bits_in_last_byte;
		const unsigned char inverse_shift = 8 - shift;

		for (int i = 0; i < addendum_bytes; i++)
		{
			// clean up new byte in case its contains garbage
			ptr[i + 0] = 0;

			ptr[i - 1] |= addendum[i] >> inverse_shift;
			ptr[i + 0] |= addendum[i] << shift;
		}
	}
	else
	{
		memcpy(buffer->data + buffer->used_bits_count / 8, addendum, addendum_bytes);
	}

	// update used bytes
	buffer->used_bits_count += addendum_bits;

	// clean unused bits in last byte
	free_bits_in_last_byte = INVERSE_MOD(buffer->used_bits_count, 8);
	if (buffer->used_bits_count > 0 && free_bits_in_last_byte > 0)
	{
		const unsigned char last_byte_mask = (0xff >> free_bits_in_last_byte) << free_bits_in_last_byte;
		buffer->data[buffer->used_bits_count / 8] &= last_byte_mask;
	}

	return 0;
}

int bitbuffer_w_write(struct bitbuffer_w *buffer, const void *addendum, size_t addendum_bits_count)
{
	return bitbuffer_w_write_(buffer, addendum, addendum_bits_count);
}

int bitbuffer_w_skip(struct bitbuffer_w *buffer, size_t bits_count)
{
	RET_IF_NZ(bitbuffer_resize_if_needed(buffer, CEIL(buffer->used_bits_count + bits_count, 8)))

	buffer->used_bits_count += bits_count;
	return 0;
}

void bitbuffer_w_dump(struct bitbuffer_w *buffer)
{
	print_binary(buffer->data, buffer->used_bits_count, 0);
}

void bitbuffer_r_initialize(struct bitbuffer_r *buffer, const void *data, size_t data_bits_count)
{
	buffer->data = data;
	buffer->total_bits_count = data_bits_count;
	buffer->read_bits_count = 0;
}

void bitbuffer_r_uninitialize(struct bitbuffer_r *buffer)
{
	free((void *)buffer->data);
}

int bitbuffer_r_read_(struct bitbuffer_r *buffer, unsigned char *target_buffer, size_t target_bits)
{
	if (buffer->total_bits_count < buffer->read_bits_count + target_bits)
		return 1;

	const int shift = (int)(buffer->read_bits_count % 8);
	const int inverse_shift = 8 - shift;

	const int total_bytes = CEIL(target_bits, 8);
	const unsigned char *ptr = buffer->data + buffer->read_bits_count / 8;

	for (int i = 0; i < total_bytes; i++)
		target_buffer[i] = (ptr[i] << shift) | (ptr[i + 1] >> inverse_shift);

	const int unread_bits_in_last_byte = INVERSE_MOD(target_bits, 8);
	target_buffer[total_bytes - 1] &= 0xffu << unread_bits_in_last_byte;

	buffer->read_bits_count += target_bits;
	return 0;
}

int bitbuffer_r_read(struct bitbuffer_r *buffer, void *target_buffer, size_t target_bits_count)
{
	return bitbuffer_r_read_(buffer, target_buffer, target_bits_count);
}

size_t bitbuffer_r_unread_count(struct bitbuffer_r *buffer)
{
	return buffer->total_bits_count - buffer->read_bits_count;
}

void bitbuffer_r_dump(struct bitbuffer_r *buffer)
{
	print_binary(buffer->data, buffer->total_bits_count, buffer->read_bits_count);
}