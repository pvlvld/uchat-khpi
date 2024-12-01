#include "../../inc/header.h"

int verify_key_pair(void) {
    int result = 0;

    BIO *private_bio = BIO_new_mem_buf((void*)vendor.crypto.private_key_str, -1);
    BIO *public_bio = BIO_new_mem_buf((void*)vendor.crypto.public_key_str, -1);

    if (!private_bio || !public_bio) {
        return 0;
    }

    EVP_PKEY *private_key = PEM_read_bio_PrivateKey(private_bio, NULL, NULL, NULL);
    if (!private_key) {
        BIO_free(private_bio);
        BIO_free(public_bio);
        return 0;
    }

    EVP_PKEY *public_key = PEM_read_bio_PUBKEY(public_bio, NULL, NULL, NULL);
    if (!public_key) {
        EVP_PKEY_free(private_key);
        BIO_free(private_bio);
        BIO_free(public_bio);
        return 0;
    }

    if (EVP_PKEY_eq(private_key, public_key) == 1) {
        if (vendor.debug_mode == 1) printf("[DEBUG] Public and private keys match!\n");
        result = 1;
    } else {
        if (vendor.debug_mode >= 1) printf("[INFO] Public and private keys do not match.\n");
        result = 0;
    }

    EVP_PKEY_free(private_key);
    EVP_PKEY_free(public_key);
    BIO_free(private_bio);
    BIO_free(public_bio);

    return result;
}
