#include "../../../libraries/openssl/evp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *hash_password(const char *password) {
    EVP_MD_CTX *mdctx;
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_length;

    // Create and initialize the context
    mdctx = EVP_MD_CTX_new();
    if (mdctx == NULL) {
        printf("Failed to create EVP_MD_CTX\n");
        return NULL;
    }

    // Initialize the digest context for SHA-256
    if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) != 1) {
        printf("EVP_DigestInit_ex failed\n");
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Update the digest with the password
    if (EVP_DigestUpdate(mdctx, password, strlen(password)) != 1) {
        printf("EVP_DigestUpdate failed\n");
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Finalize the digest
    if (EVP_DigestFinal_ex(mdctx, hash, &hash_length) != 1) {
        printf("EVP_DigestFinal_ex failed\n");
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Allocate memory for the output hash string (hex representation)
    char *hashed_output = (char *)malloc((hash_length * 2) + 1);
    if (hashed_output == NULL) {
        printf("Failed to allocate memory for hashed output\n");
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Convert the hash to a hexadecimal string
    for (unsigned int i = 0; i < hash_length; i++) { sprintf(hashed_output + (i * 2), "%02x", hash[i]); }
    hashed_output[hash_length * 2] = '\0';

    // Clean up
    EVP_MD_CTX_free(mdctx);

    return hashed_output;
}
