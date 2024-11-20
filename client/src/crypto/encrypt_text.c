#include "../../inc/header.h"

char *encrypt_text(const char *text, const char *secret) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        printf("Error creating EVP context\n");
        return NULL;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, (unsigned char*)secret, NULL) != 1) {
        printf("Error initializing encryption\n");
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    int len = strlen(text);
    int ciphertext_len = len + AES_BLOCK_SIZE;
    unsigned char* ciphertext = malloc(ciphertext_len);

    int out_len = 0;
    if (EVP_EncryptUpdate(ctx, ciphertext, &out_len, (unsigned char*)text, len) != 1) {
        printf("Error during encryption\n");
        EVP_CIPHER_CTX_free(ctx);
        free(ciphertext);
        return NULL;
    }
    ciphertext_len = out_len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext + out_len, &out_len) != 1) {
        printf("Error during final encryption\n");
        EVP_CIPHER_CTX_free(ctx);
        free(ciphertext);
        return NULL;
    }

    ciphertext_len += out_len;
    EVP_CIPHER_CTX_free(ctx);

    char* encrypted_text = malloc(ciphertext_len * 2 + 1);
    for (int i = 0; i < ciphertext_len; i++) {
        sprintf(encrypted_text + i * 2, "%02x", ciphertext[i]);
    }
    encrypted_text[ciphertext_len * 2] = '\0';

    free(ciphertext);
    return encrypted_text;
}
