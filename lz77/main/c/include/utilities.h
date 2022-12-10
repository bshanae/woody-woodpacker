#ifndef UTILITIES
#define UTILITIES

#include <stddef.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) MAX(MIN(x, max), min)
#define CEIL(a, b) (((a) / (b)) + (((a) % (b)) > 0 ? 1 : 0))
#define INVERSE_MOD(a, b) ((a) % (b) == 0 ? (0) : (((a) / (b) + 1) * (b) - (a)))

#define RET_IF_NZ(expression) if ((expression) != 0) return 1;

#endif
