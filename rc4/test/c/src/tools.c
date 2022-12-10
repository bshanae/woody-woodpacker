#include <stdlib.h>
#include <stdio.h>

void print_hex(const unsigned char *text, long size)
{
	for (size_t i = 0, len = size; i < len; i++)
		printf("0x%02hhx ", text[i]);
	printf("\n");
}