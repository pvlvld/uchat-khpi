#include "../../inc/json_web_token.h"

// Function for encoding in base64url
char *_base64url_encode(const unsigned char *data, size_t length) {
    BIO *b64, *bio;
    BUF_MEM *bptr;
    char *buff;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bio);

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, data, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    buff = malloc(bptr->length + 1);
    if (buff == NULL) {
        BIO_free_all(b64);
        return NULL;
    }
    memcpy(buff, bptr->data, bptr->length);
    buff[bptr->length] = '\0';

    // Replacing '+' with '-' and '/' with '_' for base64url
    for (char *p = buff; *p; p++) {
        if (*p == '+')
            *p = '-';
        else if (*p == '/')
            *p = '_';
        else if (*p == '=')
            *p = '\0';
    }

    BIO_free_all(b64);
    return buff;
}

// Function for creating HMAC SHA256 signature
unsigned char *_sign_hmac_sha256(const char *key, const char *data, unsigned int *len) {
    return HMAC(EVP_sha256(), key, strlen(key), (unsigned char *)data, strlen(data), NULL, len);
}

// Function for base64url decoding
unsigned char *_base64url_decode(const char *input, size_t *length) {
    size_t len = strlen(input);
    size_t padding = len % 4 == 0 ? 0 : 4 - (len % 4);
    size_t new_len = len + padding;
    char *buffer = malloc(new_len + 1);

    if (!buffer) return NULL;

    strcpy(buffer, input);
    for (size_t i = 0; i < padding; i++) buffer[len + i] = '=';
    buffer[new_len] = '\0';

    BIO *b64, *bio;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(buffer, -1);
    bio = BIO_push(b64, bio);

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    unsigned char *output = malloc(new_len);
    *length = BIO_read(b64, output, new_len);

    BIO_free_all(b64);
    free(buffer);

    return output;
}
