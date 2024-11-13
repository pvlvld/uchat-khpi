#include <openssl/sha.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

void sha256(unsigned char *data, unsigned char *outputBuffer) { SHA256(data, strlen((char *)data), outputBuffer); }
