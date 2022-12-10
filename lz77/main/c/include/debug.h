#ifndef DEBUG
#define DEBUG

#include <stddef.h>

//#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define D(expr) expr;
#define DLOG(format, ...) printf(format"\n", ##__VA_ARGS__);
#else
#define D(expr)
#define DLOG(expr, ...)
#endif

void print_binary(const void *data, size_t bits_count, size_t skip);

#endif
