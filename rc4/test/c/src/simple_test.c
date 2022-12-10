#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rc4.h"
#include "tools.h"

int simple_test()
{
	char *key = "123";
	char *text = "hello there";
	char ciphertext[11];
	char decrypted_text[11];

	rc4(key, strlen(key), text, ciphertext, 11);
	rc4(key, strlen(key), ciphertext, decrypted_text, 11);

	if (memcmp(text, decrypted_text, 11) == 0)
	{
		printf("RC4 works fine.\n");
	}
	else
	{
		printf("RC4 is broken...\n");
		printf("Expected: %s\n", text);
		printf("Got: %s\n", decrypted_text);
	}

	return 0;
}