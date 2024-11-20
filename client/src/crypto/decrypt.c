#include "../../inc/header.h"

char *decrypt(char *encrypted_base64) {
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    BIO *priv_bio = BIO_new_mem_buf(vendor.crypto.private_key_str, -1);
    unsigned char *encrypted = NULL;
    unsigned char *decrypted = NULL;
    size_t encrypted_len, decrypted_len;
    char *decrypted_text = NULL;

    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *mem = BIO_new_mem_buf(encrypted_base64, -1);
    BIO_push(b64, mem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    encrypted_len = strlen(encrypted_base64) * 3 / 4;
    encrypted = (unsigned char *)malloc(encrypted_len);
    if (!encrypted) {
        fprintf(stderr, "Memory allocation error\n");
        BIO_free_all(b64);
        return NULL;
    }

    encrypted_len = BIO_read(b64, encrypted, encrypted_len);
    BIO_free_all(b64);

    if (encrypted_len <= 0) {
        fprintf(stderr, "Error decoding Base64 data\n");
        free(encrypted);
        return NULL;
    }

    if (!priv_bio) {
        fprintf(stderr, "Error creating BIO for private key\n");
        free(encrypted);
        return NULL;
    }

    pkey = PEM_read_bio_PrivateKey(priv_bio, NULL, NULL, NULL);
    BIO_free(priv_bio);

    if (!pkey) {
        fprintf(stderr, "Error loading private key\n");
        free(encrypted);
        return NULL;
    }

    ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx || EVP_PKEY_decrypt_init(ctx) <= 0) {
        fprintf(stderr, "Error initializing decryption context\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        free(encrypted);
        return NULL;
    }

    if (EVP_PKEY_decrypt(ctx, NULL, &decrypted_len, encrypted, encrypted_len) <= 0) {
        fprintf(stderr, "Error determining decrypted length\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        free(encrypted);
        return NULL;
    }

    decrypted = (unsigned char *)malloc(decrypted_len + 1);
    if (!decrypted) {
        fprintf(stderr, "Memory allocation error\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        free(encrypted);
        return NULL;
    }

    if (EVP_PKEY_decrypt(ctx, decrypted, &decrypted_len, encrypted, encrypted_len) <= 0) {
        fprintf(stderr, "Error decrypting data\n");
        free(decrypted);
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        free(encrypted);
        return NULL;
    }

    decrypted[decrypted_len] = '\0';
    decrypted_text = (char *)decrypted;

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    free(encrypted);

    return decrypted_text;
}
