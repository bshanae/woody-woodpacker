#include "bitbuffer.h"
#include "utilities.h"
#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

// BYTEARRAY //////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct sized_bytearray
{
	size_t size;
	unsigned char *ptr;
};

static struct sized_bytearray *allocate_sized_bytearray(int count, ...)
{
	struct sized_bytearray *bytearray = malloc(sizeof(struct sized_bytearray));
	bytearray->size = count;
	bytearray->ptr = malloc(count);

	va_list args;
    va_start(args, count);

	for (int i = 0; i < count; i++)
		bytearray->ptr[i] = (unsigned char)va_arg(args, int);

    va_end(args);

	return bytearray;
}

static void deallocate_sized_bytearray(struct sized_bytearray *bytearray)
{
	free(bytearray->ptr);
	free(bytearray);
}

#define BYTEARRAY(count, ...) allocate_bytearray(count, ##__VA_ARGS__)

// TOOLS //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int cmp_bits(const unsigned char *reference, const unsigned char *result, size_t bits_count)
{
	const size_t bytes_count = bits_count / 8;
	const size_t lone_bites_count = bits_count % 8;

	if (memcmp(reference, result, bytes_count) != 0)
		return 1;

	const unsigned char last_byte_mask = 0xff >> (8 - lone_bites_count);
	const unsigned char last_reference_byte = reference[bytes_count] & last_byte_mask;
	const unsigned char last_result_byte = result[bytes_count] & last_byte_mask;
	if (last_reference_byte != last_result_byte)
		return 1;

	return 0;
}

// BITBUFFER_W TEST ///////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void bitbuffer_w_test_finalize(const int test_id, struct sized_bytearray *reference, struct bitbuffer_w *result)
{
	if (cmp_bits(reference->ptr, result->data, result->used_bits_count) != 0)
	{
		printf("[bitbuffer_w %d] Expected: ", test_id);
		print_binary(reference->ptr, reference->size * 8, 0);
		printf("[bitbuffer_w %d] Got:      ", test_id);
		bitbuffer_w_dump(result);
	}

	deallocate_sized_bytearray(reference);
}

#define BITBUFFER_W_TEST_START(x) \
static void bitbuffer_w_test_##x() \
{ \
    const int test_id = x; \
	struct bitbuffer_w buffer; \
	bitbuffer_w_initialize(&buffer, 10); \

#define BITBUFFER_W_TEST_EXPECT(count, ...) struct sized_bytearray *reference = allocate_sized_bytearray((count), ##__VA_ARGS__);

#define BITBUFFER_W_TEST_END \
	bitbuffer_w_test_finalize(test_id, reference, &buffer); \
}

#define BITBUFFER_W_TEST_EVAL(x) bitbuffer_w_test_##x();

// BITBUFFER_R TEST ///////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void bitbuffer_r_test_finalize(const int test_id, struct sized_bytearray *reference, const unsigned char *result)
{
	if (cmp_bits(reference->ptr, result, reference->size * 8) != 0)
	{
		printf("[bitbuffer_r %d] Expected: ", test_id);
		print_binary(reference->ptr, reference->size * 8, 0);
		printf("[bitbuffer_r %d] Got:      ", test_id);
		print_binary(result, reference->size * 8, 0);
	}

	deallocate_sized_bytearray(reference);
}

#define BITBUFFER_R_TEST_START(x) \
void bitbuffer_r_test_##x() \
{ \
    const int test_id = x; \
	unsigned char test[100]; \
	struct bitbuffer_r buffer; \

#define BITBUFFER_R_TEST_LOAD_BITS(bits_count, ...) \
    struct sized_bytearray *data_array  = allocate_sized_bytearray(CEIL(bits_count, 8), ##__VA_ARGS__); \
	bitbuffer_r_initialize(&buffer, data_array->ptr, bits_count);

#define BITBUFFER_R_TEST_READ(bits) \
	memset(test, 0, 100); \
	bitbuffer_r_read(&buffer, &test, bits);

#define BITBUFFER_R_TEST_EXPECT(count, ...) struct sized_bytearray *reference = allocate_sized_bytearray((count), ##__VA_ARGS__);

#define BITBUFFER_R_TEST_END \
	bitbuffer_r_test_finalize(test_id, reference, test); \
	deallocate_sized_bytearray(data_array); \
}

#define BITBUFFER_R_TEST_EVAL(x) bitbuffer_r_test_##x();

// TESTS //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BITBUFFER_W_TEST_START(0)
	unsigned char addendum = 0b10000000u;
	bitbuffer_w_write(&buffer, &addendum, 1);

	BITBUFFER_W_TEST_EXPECT(1, 0b10000000u)
BITBUFFER_W_TEST_END

BITBUFFER_W_TEST_START(1)
	unsigned char addendum = 0b10000000u;

	bitbuffer_w_write(&buffer, &addendum, 1);
	bitbuffer_w_write(&buffer, &addendum, 1);

	BITBUFFER_W_TEST_EXPECT(1, 0b11000000u)
BITBUFFER_W_TEST_END

BITBUFFER_W_TEST_START(2)
	unsigned char addendum = 0b10000000u;
	bitbuffer_w_write(&buffer, &addendum, 1);

	addendum = 0b00000000u;
	bitbuffer_w_write(&buffer, &addendum, 1);

	BITBUFFER_W_TEST_EXPECT(1, 0b10000000u)
BITBUFFER_W_TEST_END

BITBUFFER_W_TEST_START(3)
	unsigned char addendum = 0b11110000u;

	bitbuffer_w_write(&buffer, &addendum, 4);
	bitbuffer_w_write(&buffer, &addendum, 4);

	BITBUFFER_W_TEST_EXPECT(1, 0b11111111u)
BITBUFFER_W_TEST_END

BITBUFFER_W_TEST_START(4)
	unsigned char addendum = 0b11111100u;

	bitbuffer_w_write(&buffer, &addendum, 6);
	bitbuffer_w_write(&buffer, &addendum, 6);

	BITBUFFER_W_TEST_EXPECT(2, 0b11111111u, 0b11110000u)
BITBUFFER_W_TEST_END

BITBUFFER_W_TEST_START(5)
	unsigned char addendum = 0b11111100u;

	bitbuffer_w_write(&buffer, &addendum, 6);
	bitbuffer_w_write(&buffer, &addendum, 6);
	bitbuffer_w_write(&buffer, &addendum, 6);
	bitbuffer_w_write(&buffer, &addendum, 6);

	BITBUFFER_W_TEST_EXPECT(3, 0b11111111u, 0b11111111u, 0b11111111u)
BITBUFFER_W_TEST_END

BITBUFFER_W_TEST_START(6)
	unsigned char addendum = 0b10101010u;

	bitbuffer_w_write(&buffer, &addendum, 3);
	bitbuffer_w_write(&buffer, &addendum, 4);
	bitbuffer_w_write(&buffer, &addendum, 7);

	BITBUFFER_W_TEST_EXPECT(2, 0b10110101u, 0b01010100)
BITBUFFER_W_TEST_END

BITBUFFER_W_TEST_START(7)
	unsigned char addendum[] = { 0b10001000u, 0b10001000u };

	bitbuffer_w_write(&buffer, addendum, 10);
	bitbuffer_w_write(&buffer, addendum, 4);

	BITBUFFER_W_TEST_EXPECT(2, 0b10001000u, 0b10100000u)
BITBUFFER_W_TEST_END

BITBUFFER_W_TEST_START(8)
	bitbuffer_w_skip(&buffer, sizeof(size_t) * 8);
	memset(buffer.data, 0l, sizeof(size_t));

	size_t addendum1 = 1;
	bitbuffer_w_write(&buffer, &addendum1, sizeof(addendum1) * 8);

	unsigned char addendum2[] = { 0b10101010u };

	bitbuffer_w_write(&buffer, addendum2, 1);
	bitbuffer_w_write(&buffer, addendum2, 8);

	bitbuffer_w_write(&buffer, addendum2, 1);
	bitbuffer_w_write(&buffer, addendum2, 8);

	BITBUFFER_W_TEST_EXPECT(
		8 + 8 + 3,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b00000001, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b11010101, 0b01101010, 0b10000000
	)
BITBUFFER_W_TEST_END

BITBUFFER_W_TEST_START(9)
	unsigned char padding[] = { 0b11111111u };
	bitbuffer_w_write(&buffer, padding, 2);

	unsigned char addendum[] = { 0b10001000u, 0b11111111u };
	bitbuffer_w_write(&buffer, addendum, 12);

	BITBUFFER_W_TEST_EXPECT(2, 0b11100010u, 0b00111100u)
BITBUFFER_W_TEST_END

BITBUFFER_W_TEST_START(10)
	unsigned char padding[] = { 0b11111111u };
	bitbuffer_w_write(&buffer, padding, 2);

	unsigned char addendum[] = { 0b10000000u, 0b10001000u, 0b10101010u };
	bitbuffer_w_write(&buffer, addendum, 20);

	BITBUFFER_W_TEST_EXPECT(3, 0b11100000u, 0b00100010u, 0b00101000u)
BITBUFFER_W_TEST_END

BITBUFFER_R_TEST_START(100)
	BITBUFFER_R_TEST_LOAD_BITS(1, 0b10000000u)

	BITBUFFER_R_TEST_READ(1)
	BITBUFFER_R_TEST_EXPECT(1, 0b10000000u)
BITBUFFER_R_TEST_END

BITBUFFER_R_TEST_START(101)
	BITBUFFER_R_TEST_LOAD_BITS(2, 0b11000000u)

	BITBUFFER_R_TEST_READ(2)
	BITBUFFER_R_TEST_EXPECT(1, 0b11000000u)
BITBUFFER_R_TEST_END

BITBUFFER_R_TEST_START(102)
	BITBUFFER_R_TEST_LOAD_BITS(8, 0b10101010u)

	BITBUFFER_R_TEST_READ(2)
	BITBUFFER_R_TEST_EXPECT(1, 0b10000000u)
BITBUFFER_R_TEST_END

BITBUFFER_R_TEST_START(103)
	BITBUFFER_R_TEST_LOAD_BITS(8, 0b10101010u)

	BITBUFFER_R_TEST_READ(2)
	BITBUFFER_R_TEST_READ(2)

	BITBUFFER_R_TEST_EXPECT(1, 0b10000000u)
BITBUFFER_R_TEST_END

BITBUFFER_R_TEST_START(104)
	BITBUFFER_R_TEST_LOAD_BITS(16, 0b10011001u, 0b10011001u)

	BITBUFFER_R_TEST_READ(8)
	BITBUFFER_R_TEST_EXPECT(1, 0b10011001u)
BITBUFFER_R_TEST_END

BITBUFFER_R_TEST_START(105)
	BITBUFFER_R_TEST_LOAD_BITS(16, 0b10011001u, 0b10011001u)

	BITBUFFER_R_TEST_READ(8)
	BITBUFFER_R_TEST_READ(8)

	BITBUFFER_R_TEST_EXPECT(1, 0b10011001u)
BITBUFFER_R_TEST_END

BITBUFFER_R_TEST_START(106)
	BITBUFFER_R_TEST_LOAD_BITS(16, 0b10011001u, 0b10011001u)

	BITBUFFER_R_TEST_READ(12)
	BITBUFFER_R_TEST_EXPECT(2, 0b10011001u, 0b10010000u)
BITBUFFER_R_TEST_END

void bitbuffer_test_suite()
{
	BITBUFFER_W_TEST_EVAL(0)
	BITBUFFER_W_TEST_EVAL(1)
	BITBUFFER_W_TEST_EVAL(2)
	BITBUFFER_W_TEST_EVAL(3)
	BITBUFFER_W_TEST_EVAL(4)
	BITBUFFER_W_TEST_EVAL(5)
	BITBUFFER_W_TEST_EVAL(6)
	BITBUFFER_W_TEST_EVAL(7)
	BITBUFFER_W_TEST_EVAL(8)
	BITBUFFER_W_TEST_EVAL(9)
	BITBUFFER_W_TEST_EVAL(10)

	BITBUFFER_R_TEST_EVAL(100)
	BITBUFFER_R_TEST_EVAL(101)
	BITBUFFER_R_TEST_EVAL(102)
	BITBUFFER_R_TEST_EVAL(103)
	BITBUFFER_R_TEST_EVAL(104)
	BITBUFFER_R_TEST_EVAL(105)
	BITBUFFER_R_TEST_EVAL(106)
}