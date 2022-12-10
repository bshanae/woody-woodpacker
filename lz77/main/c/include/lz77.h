#ifndef LZ77
#define LZ77

#include <stddef.h>
#include "bitbuffer.h"

int lz77_compress(unsigned char **compressed_data_raw, size_t *compressed_data_bits_count, const unsigned char *data, size_t data_bytes_count);
int lz77_decompress(unsigned char **data_ptr, const unsigned char *compressed_data_raw);

#endif
