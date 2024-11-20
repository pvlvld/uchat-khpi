#include "../../inc/header.h"

char *decrypt_text(const char *encrypted, const char *secret) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        printf("Error creating EVP context\n");
        return NULL;
    }

//    if (strlen(secret) < 16) {
//        printf("Error: Secret key too short\n");
//        EVP_CIPHER_CTX_free(ctx);
//        return NULL;
//    }

    int encrypted_len = strlen(encrypted) / 2;
    unsigned char* ciphertext = malloc(encrypted_len);
    if (!ciphertext) {
        printf("Error allocating memory for ciphertext\n");
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    for (int i = 0; i < encrypted_len; i++) {
        sscanf(encrypted + i * 2, "%2hhx", &ciphertext[i]);
    }

    unsigned char iv[AES_BLOCK_SIZE];
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        sscanf(encrypted + i * 2, "%2hhx", &iv[i]);
    }

    int ciphertext_len = encrypted_len - AES_BLOCK_SIZE;
    unsigned char* data = malloc(ciphertext_len);
    if (!data) {
        printf("Error allocating memory for decrypted data\n");
        EVP_CIPHER_CTX_free(ctx);
        free(ciphertext);
        return NULL;
    }

    for (int i = 0; i < ciphertext_len; i++) {
        sscanf(encrypted + (i + AES_BLOCK_SIZE) * 2, "%2hhx", &data[i]);
    }

    int plaintext_len = ciphertext_len;
    unsigned char* plaintext = malloc(plaintext_len + 1);
    if (!plaintext) {
        printf("Error allocating memory for plaintext\n");
        EVP_CIPHER_CTX_free(ctx);
        free(data);
        free(ciphertext);
        return NULL;
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, (unsigned char*)secret, iv) != 1) {
        printf("Error initializing decryption\n");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        free(data);
        free(ciphertext);
        return NULL;
    }

    int out_len = 0;
    if (EVP_DecryptUpdate(ctx, plaintext, &out_len, data, ciphertext_len) != 1) {
        printf("Error during decryption\n");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        free(data);
        free(ciphertext);
        return NULL;
    }
    plaintext_len = out_len;

    if (EVP_DecryptFinal_ex(ctx, plaintext + out_len, &out_len) != 1) {
        printf("Error during final decryption\n");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        free(data);
        free(ciphertext);
        return NULL;
    }

    plaintext_len += out_len;
    EVP_CIPHER_CTX_free(ctx);

    plaintext[plaintext_len] = '\0';
    free(data);
    free(ciphertext);

    return (char*)plaintext;
}
