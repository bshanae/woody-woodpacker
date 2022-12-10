#include "debug.h"
#include <stdlib.h>
#include <stdio.h>
#include "utilities.h"

void print_binary(const void *data, size_t bits_count, size_t skip)
{
	const size_t bytes_count = CEIL(bits_count, 8);
	const int unused_bits_in_last_byte = INVERSE_MOD(bits_count, 8);

	char *string = malloc(bytes_count * (8 + 1) + 1);

	size_t string_i = 0;
	size_t byte_i = 0;
	for (; byte_i < bytes_count; byte_i++)
	{
		const unsigned char byte = ((const unsigned char *)data)[byte_i];
		for (int j = 7; j >= 0; j--)
		{
			if (skip > 0)
			{
				skip--;
				continue;
			}

			string[string_i++] = (byte & (1 << j)) ? '1' : '0';
		}

		if (string_i > 0)
			string[string_i++] = ' ';
	}
	if (byte_i > 0)
	{
		for (int j = unused_bits_in_last_byte; j > 0; j--)
			string[string_i - 1 - j] = ' ';
	}
	string[string_i++] = '\0';

	printf("%s\n", string);
	free(string);
}