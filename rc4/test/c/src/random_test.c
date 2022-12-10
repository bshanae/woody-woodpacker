#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "rc4.h"
#include "rc4_reference.h"
#include "tools.h"

#define KEY_MAX_SIZE 512
#define PLAINTEXT_MAX_SIZE 64

typedef struct
{
	size_t size;
	char *ptr;
} sized_string;

void compare_texts(const unsigned char *expected, const unsigned char *got, long size)
{
	if (memcmp(expected, got, size) != 0)
	{
		printf("Expected:\n");
		print_hex(expected, size);

		printf("Got:\n");
		print_hex(got, size);
	}
	else
	{
		printf("Texts are equal.\n");
	}
}

static char *rand_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
    return str;
}

static sized_string rand_string2(size_t max_size)
{
	size_t size = rand() % max_size + 2;
	char *buffer = malloc(size);

	rand_string(buffer, size);

	return (sized_string){ size, buffer };
}

void test1()
{
	sized_string key = rand_string2(KEY_MAX_SIZE);
	sized_string plaintext = rand_string2(PLAINTEXT_MAX_SIZE);

	// printf("Key: %ld, %p, %s\n", key.size, key.ptr, key.ptr);
	// printf("Text: %ld, %p, %s\n\n", plaintext.size, plaintext.ptr, plaintext.ptr);

	unsigned char *ciphertext_reference = calloc(1, plaintext.size + 1);
	unsigned char *ciphertext_actual = calloc(1, plaintext.size + 1);

	// printf("cipertext: %p\n", ciphertext_actual);

	rc4_reference(key.ptr, key.size, plaintext.ptr, ciphertext_reference, plaintext.size);
	rc4(key.ptr, key.size, plaintext.ptr, ciphertext_actual, plaintext.size);

	compare_texts(ciphertext_reference, ciphertext_actual, plaintext.size);
}

void test2()
{
	sized_string key = rand_string2(KEY_MAX_SIZE);
	sized_string plaintext = rand_string2(PLAINTEXT_MAX_SIZE);

	// printf("Key: (%ld) %s\n", key.size, key.ptr);
	// printf("Text: (%ld) %s\n\n", plaintext.size, plaintext.ptr);

	unsigned char *ciphertext = calloc(1, plaintext.size + 1);
	unsigned char *plaintext_decrypted = calloc(1, plaintext.size + 1);

	rc4_reference(key.ptr, key.size, plaintext.ptr, ciphertext, plaintext.size);
	rc4_reference(key.ptr, key.size, ciphertext, plaintext_decrypted, plaintext.size);

	// printf("Decrypted text: %s\n\n", plaintext_decrypted);

	compare_texts(plaintext.ptr, plaintext_decrypted, plaintext.size);
}

int random_test(const char *seed_str)
{
	long seed;
	if (seed_str != NULL)
	{
		seed = atoi(seed_str);
	}
	else
	{
		struct timeval tm;
		gettimeofday(&tm, NULL);
		seed = (tm.tv_sec + tm.tv_usec * 1000000ul) >> 5u;
	}

	printf("Seed: %ld\n", seed);
	// printf("\n");

	srand(seed);

	// test1();
	test2();

	return 0;
}