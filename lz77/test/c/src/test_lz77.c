#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lz77.h"
#include "debug.h"

const char *delimiter = "/-------------------------------------------------------------------------/";

void lz77_test(const int test_id, const unsigned char *sample, size_t size)
{
	DLOG("%s TEST %d %s", delimiter, test_id, delimiter)

	unsigned char *compressed_data;
	size_t compressed_data_size;
	lz77_compress(&compressed_data, &compressed_data_size, (const unsigned char *) sample, size);

	DLOG("")

	char *decompressed_data;
	lz77_decompress((unsigned char **) &decompressed_data, compressed_data);

	if (memcmp(sample, decompressed_data, size) != 0)
	{
		printf("[lz77 %d] FAILED\n", test_id);
		printf("[lz77 %d] Expected: %s\n", test_id, sample);
		printf("[lz77 %d] Got     : %s\n", test_id, decompressed_data);
	}
	else
	{
		printf("[lz77 %d] OK\n", test_id);
		printf("[lz77 %d] Compression ratio: %f\n", test_id, (float)compressed_data_size / ((float)size * 8));
	}

	DLOG("%s/-------/%s", delimiter, delimiter)
	DLOG("")
	DLOG("")
}

void lz77_test_cstr(const int test_id, const char *sample)
{
	lz77_test(test_id, (const unsigned char *) sample, strlen(sample));
}

void lz77_test_file(const int test_id, const char *file_path)
{
	FILE *file = fopen(file_path, "r");

	fseek(file, 0, SEEK_END);
	long fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = malloc(fsize + 1);
	fread(buffer, fsize, 1, file);
	buffer[fsize] = 0;

	lz77_test(test_id, (const unsigned char *)buffer, fsize);

	fclose(file);
}

void lz77_test_txt(const int test_id, const char *filename)
{
	char *file_path = malloc(64);
	strcpy(file_path, "res/test_txt/");
	strcat(file_path, filename);

	lz77_test_file(test_id, file_path);

	free(file_path);
}

void lz77_test_exe(const int test_id, const char *filename)
{
	char *file_path = malloc(64);
	strcpy(file_path, "res/test_exe/");
	strcat(file_path, filename);

	lz77_test_file(test_id, file_path);

	free(file_path);
}

void lz77_test_suite()
{
	lz77_test_cstr(0, "abc");
	lz77_test_cstr(1, "aba");
	lz77_test_cstr(2, "abab");
	lz77_test_cstr(3, "aaabbb");
	lz77_test_cstr(4, "aaabbbaaa");
	lz77_test_cstr(5, "hello hello");
	lz77_test_cstr(6, "1 12 123 1234 12345 123456 1234567 12345678 123456789");
	lz77_test_cstr(7, "123456789 12345678 1234567 123456 12345 1234 123 12 1");
	lz77_test_cstr(8, "11111111111111111111111111111111111111111111111111111111111111111111111111111111");
	lz77_test_cstr(9, "1111111111111111111 1111111111111111111 1111111111111111111 1111111111111111111 ");
	lz77_test_cstr(10, "aaaaaaaa bbbbbbbb aaaaaaaa bbbbbbbb aaaaaaaa bbbbbbbb");
	lz77_test_cstr(11, "abcdefghijklmnopqrstuvwxyz");
	lz77_test_cstr(12, "a b c d e f g h i j k l m n o p q r s t u v w x y z");

	lz77_test_txt(100, "test_lorem_ipsum_1.txt");
	lz77_test_txt(101, "test_lorem_ipsum_2.txt");
	lz77_test_txt(102, "test_lorem_ipsum_3.txt");
	lz77_test_txt(103, "test_lorem_ipsum_4.txt");
	lz77_test_txt(104, "test_lorem_ipsum_5.txt");
	lz77_test_txt(105, "test_random_1.txt");
	lz77_test_txt(106, "test_random_2.txt");
	lz77_test_txt(107, "test_random_3.txt");
	lz77_test_txt(108, "test_random_4.txt");
	lz77_test_txt(109, "test_random_5.txt");
	lz77_test_txt(110, "test_great_gatsby.txt");

	lz77_test_exe(201, "ft_nm");
	lz77_test_exe(202, "ft_vox");
}