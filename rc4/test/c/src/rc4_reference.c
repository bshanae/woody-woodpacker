#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N 256   // 2^8

void swap(unsigned char *a, unsigned char *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

int ksa(const char *key, size_t keylength, unsigned char *S)
{

	int j = 0;

	for (int i = 0; i < N; i++)
		S[i] = i;

	for (int i = 0; i < N; i++)
	{
		j = (j + S[i] + key[i % keylength]) % N;

		swap(&S[i], &S[j]);
	}

	return 0;
}

int rpga(unsigned char *S, const char *plaintext, size_t plaintext_length, unsigned char *ciphertext)
{
	int i = 0;
	int j = 0;

	for (size_t n = 0, len = plaintext_length; n < len; n++)
	{
		i = (i + 1) % N;
		j = (j + S[i]) % N;

		swap(&S[i], &S[j]);
		int rnd = S[(S[i] + S[j]) % N];

		ciphertext[n] = rnd ^ plaintext[n];
	}

	return 0;
}

void rc4_reference(const char *key, size_t key_length, const char *plaintext, unsigned char *ciphertext, size_t text_length)
{
	unsigned char S[N];
	ksa(key, key_length, S);

	rpga(S, plaintext, text_length, ciphertext);
}