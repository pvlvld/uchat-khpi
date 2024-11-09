#include "../../inc/header.h"

int keygen(void) {
    int result = -1;
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    BIO *pub_bio = NULL, *priv_bio = NULL;

    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx || EVP_PKEY_keygen_init(ctx) <= 0 || EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, KEY_LENGTH) <= 0) {
        fprintf(stderr, "Error initializing context or setting key length\n");
        EVP_PKEY_CTX_free(ctx);
        return result;
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        fprintf(stderr, "Error generating RSA key\n");
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return result;
    }

    pub_bio = BIO_new(BIO_s_mem());
    priv_bio = BIO_new(BIO_s_mem());
    if (!pub_bio || !priv_bio) {
        fprintf(stderr, "Error creating BIOs\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        BIO_free(pub_bio);
        BIO_free(priv_bio);
        return result;
    }

    if (!PEM_write_bio_PUBKEY(pub_bio, pkey)) {
        fprintf(stderr, "Error writing public key to BIO\n");
    } else if (!PEM_write_bio_PrivateKey(priv_bio, pkey, NULL, NULL, 0, NULL, NULL)) {
        fprintf(stderr, "Error writing private key to BIO\n");
    } else {
        int pub_len = BIO_pending(pub_bio);
        int priv_len = BIO_pending(priv_bio);

        if (vendor.crypto.public_key_str != NULL) {
            free(vendor.crypto.public_key_str);
        }
        if (vendor.crypto.private_key_str != NULL) {
            free(vendor.crypto.private_key_str);
        }

        vendor.crypto.public_key_str = (char *)malloc(pub_len + 1);
        vendor.crypto.private_key_str = (char *)malloc(priv_len + 1);

        if (vendor.crypto.public_key_str && vendor.crypto.private_key_str) {
            BIO_read(pub_bio, vendor.crypto.public_key_str, pub_len);
            BIO_read(priv_bio, vendor.crypto.private_key_str, priv_len);

            vendor.crypto.public_key_str[pub_len] = '\0';
            vendor.crypto.private_key_str[priv_len] = '\0';
            result = 0;
        } else {
            fprintf(stderr, "Error allocating memory for keys\n");
            free(vendor.crypto.public_key_str);
            free(vendor.crypto.private_key_str);
        }
    }

    BIO_free(pub_bio);
    BIO_free(priv_bio);
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);

    return result;
}
