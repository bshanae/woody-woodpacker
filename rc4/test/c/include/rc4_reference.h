#ifndef RC4_REFERENCE
#define RC4_REFERENCE

void rc4_reference(const char *key, size_t key_length, const char *plaintext, unsigned char *ciphertext, size_t text_length);

#endif