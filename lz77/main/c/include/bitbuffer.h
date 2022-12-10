#ifndef BITBUFFER
#define BITBUFFER

#include <stddef.h>

struct bitbuffer_w
{
	unsigned char *data;
	size_t allocated_bytes_count;
	size_t used_bits_count;
};

int bitbuffer_w_initialize(struct bitbuffer_w *buffer, size_t preallocated_bytes_count);
void bitbuffer_w_uninitialize(struct bitbuffer_w *buffer);
int bitbuffer_w_write(struct bitbuffer_w *buffer, const void *addendum, size_t addendum_bits_count);
int bitbuffer_w_skip(struct bitbuffer_w *buffer, size_t bits_count);
void bitbuffer_w_dump(struct bitbuffer_w *buffer);

struct bitbuffer_r
{
	const unsigned char *data;
	size_t total_bits_count;
	size_t read_bits_count;
};

void bitbuffer_r_initialize(struct bitbuffer_r *buffer, const void *data, size_t data_bits_count);
void bitbuffer_r_uninitialize(struct bitbuffer_r *buffer);
int bitbuffer_r_read(struct bitbuffer_r *buffer, void *target_buffer, size_t target_bits_count);
size_t bitbuffer_r_unread_count(struct bitbuffer_r *buffer);
void bitbuffer_r_dump(struct bitbuffer_r *buffer);

#endif
