#include "../../inc/header.h"

char *decrypt_text(const char *encrypted, const char *secret) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        printf("Error creating EVP context\n");
        return NULL;
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, (unsigned char*)secret, NULL) != 1) {
        printf("Error initializing decryption\n");
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    int encrypted_len = strlen(encrypted) / 2;
    unsigned char* ciphertext = malloc(encrypted_len);
    for (int i = 0; i < encrypted_len; i++) {
        sscanf(encrypted + i * 2, "%2hhx", &ciphertext[i]);
    }

    int plaintext_len = encrypted_len;
    unsigned char* plaintext = malloc(plaintext_len + 1);
    int out_len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext, &out_len, ciphertext, encrypted_len) != 1) {
        printf("Error during decryption\n");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        free(ciphertext);
        return NULL;
    }
    plaintext_len = out_len;

    if (EVP_DecryptFinal_ex(ctx, plaintext + out_len, &out_len) != 1) {
        printf("Error during final decryption\n");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        free(ciphertext);
        return NULL;
    }

    plaintext_len += out_len;
    EVP_CIPHER_CTX_free(ctx);

    plaintext[plaintext_len] = '\0';
    free(ciphertext);
    return (char*)plaintext;
}
