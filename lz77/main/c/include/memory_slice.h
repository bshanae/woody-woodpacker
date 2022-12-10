#ifndef MEMORY_SLICE
#define MEMORY_SLICE

#include <stdlib.h>

struct memory_slice
{
	// Current offset within data
	long offset;

	// Size of the slice
	size_t size;

	const void *data;
	size_t data_size;
};

void memory_slice_initialize(struct memory_slice *slice, size_t size, const void *data, size_t data_size);
const void *memory_slice_begin(struct memory_slice *slice);
const void *memory_slice_end(struct memory_slice *slice);
int memory_slice_shift_left(struct memory_slice *slice, long shift);
int memory_slice_shift_right(struct memory_slice *slice, long shift);
size_t memory_slice_data_left(struct memory_slice *slice);

#endif
