#include "../../inc/header.h"

char *encrypt_text(const char *text, const char *secret) {
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

    unsigned char iv[AES_BLOCK_SIZE];
    if (RAND_bytes(iv, AES_BLOCK_SIZE) != 1) {
        printf("Error generating IV\n");
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, (unsigned char*)secret, iv) != 1) {
        printf("Error initializing encryption\n");
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    int len = strlen(text);
    int ciphertext_len = len + AES_BLOCK_SIZE;
    unsigned char* ciphertext = malloc(ciphertext_len);
    if (!ciphertext) {
        printf("Error allocating memory for ciphertext\n");
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

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

    char* encrypted_text = malloc(ciphertext_len * 2 + AES_BLOCK_SIZE * 2 + 1);
    if (!encrypted_text) {
        printf("Error allocating memory for encrypted text\n");
        free(ciphertext);
        return NULL;
    }

    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        snprintf(encrypted_text + i * 2, 3, "%02x", iv[i]);
    }

    for (int i = 0; i < ciphertext_len; i++) {
        snprintf(encrypted_text + AES_BLOCK_SIZE * 2 + i * 2, 3, "%02x", ciphertext[i]);
    }

    encrypted_text[ciphertext_len * 2 + AES_BLOCK_SIZE * 2] = '\0';

    free(ciphertext);
    return encrypted_text;
}
