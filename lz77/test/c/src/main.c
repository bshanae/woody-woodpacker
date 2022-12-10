#include "test_lz77.h"
#include "test_bitbuffer.h"

int main()
{
	bitbuffer_test_suite();
	lz77_test_suite();
}