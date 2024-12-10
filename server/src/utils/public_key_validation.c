#include "../../../libraries/openssl/evp.h"
#include "../../../libraries/openssl/pem.h"
#include "../../../libraries/openssl/err.h"
#include <string.h>
#include <stdio.h>

// The length of the public key string must be between 200 and 450 characters.
// The key must be in PEM format.
// The key must be an RSA key.
// The RSA key size must be 1024 bits.
// Must be parsable by OpenSSL without errors.

int is_valid_public_key(const char *public_key_str) {
    BIO *bio = NULL;
    EVP_PKEY *pkey = NULL;
    int result = 0;

    if (strlen(public_key_str) < 200 || strlen(public_key_str) > 450) {
        fprintf(stderr, "Invalid key length\n");
        return 0;
    }

    bio = BIO_new_mem_buf(public_key_str, -1);
    if (!bio) {
        fprintf(stderr, "Failed to create BIO\n");
        return 0;
    }

    // Read the public key in PEM format
    pkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
    if (!pkey) {
        fprintf(stderr, "Failed to load public key: %s\n", ERR_error_string(ERR_get_error(), NULL));
        BIO_free(bio);
        return 0;
    }

    // Check if the key is an RSA key and check its length
    if (EVP_PKEY_base_id(pkey) == EVP_PKEY_RSA) {
        int key_bits = EVP_PKEY_bits(pkey);
        if (key_bits == 1024) {
            result = 1;
        } else {
            fprintf(stderr, "Invalid key size: %d bits\n", key_bits);
        }
    } else {
        fprintf(stderr, "Invalid key type\n");
    }

    EVP_PKEY_free(pkey);
    BIO_free(bio);

    return result;
}
