#include "../../inc/header.h"

char *encrypt(char *public_key_str, const char *data) {
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    BIO *pub_bio = BIO_new_mem_buf(public_key_str, -1);
    unsigned char *encrypted = NULL;
    size_t encrypted_len;
    char *base64_encrypted = NULL;

    if (!pub_bio) {
        fprintf(stderr, "Error creating BIO for public key\n");
        return NULL;
    }

    pkey = PEM_read_bio_PUBKEY(pub_bio, NULL, NULL, NULL);
    BIO_free(pub_bio);

    if (!pkey) {
        fprintf(stderr, "Error loading public key\n");
        return NULL;
    }

    ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx || EVP_PKEY_encrypt_init(ctx) <= 0) {
        fprintf(stderr, "Error initializing encryption context\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_encrypt(ctx, NULL, &encrypted_len, (unsigned char *)data, strlen(data)) <= 0) {
        fprintf(stderr, "Error determining encrypted length\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    encrypted = (unsigned char *)malloc(encrypted_len);
    if (!encrypted) {
        fprintf(stderr, "Memory allocation error\n");
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_encrypt(ctx, encrypted, &encrypted_len, (unsigned char *)data, strlen(data)) <= 0) {
        fprintf(stderr, "Error encrypting data\n");
        free(encrypted);
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);

    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *mem = BIO_new(BIO_s_mem());
    BIO_push(b64, mem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    BIO_write(b64, encrypted, encrypted_len);
    BIO_flush(b64);

    BUF_MEM *buffer_ptr;
    BIO_get_mem_ptr(mem, &buffer_ptr);

    base64_encrypted = (char *)malloc(buffer_ptr->length + 1);
    if (base64_encrypted) {
        memcpy(base64_encrypted, buffer_ptr->data, buffer_ptr->length);
        base64_encrypted[buffer_ptr->length] = '\0';
    }

    BIO_free_all(b64);
    free(encrypted);

    return base64_encrypted;
}
