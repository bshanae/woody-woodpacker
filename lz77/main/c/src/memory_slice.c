#include "memory_slice.h"
#include "utilities.h"

void memory_slice_initialize(struct memory_slice *slice, size_t size, const void *data, size_t data_size)
{
	slice->offset = 0;
	slice->size = size;
	slice->data = data;
	slice->data_size = data_size;
}

const void *memory_slice_begin(struct memory_slice *slice)
{
	const void *ptr = slice->data + slice->offset;
	return CLAMP(ptr, slice->data, slice->data + slice->data_size);
}

const void *memory_slice_end(struct memory_slice *slice)
{
	const void *ptr = slice->data + slice->offset + slice->size;
	return CLAMP(ptr, slice->data, slice->data + slice->data_size);
}

int memory_slice_shift_left(struct memory_slice *slice, long shift)
{
	slice->offset -= shift;
	return 0;
}

int memory_slice_shift_right(struct memory_slice *slice, long shift)
{
	slice->offset += shift;
	return 0;
}

size_t memory_slice_data_left(struct memory_slice *slice)
{
	if (slice->data_size < slice->offset)
		return 0;

	return slice->data_size - slice->offset;
}