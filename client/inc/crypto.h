#ifndef CRYPTO_H
#define CRYPTO_H

#include "header.h"

#include "../../libraries/openssl/rsa.h"
#include "../../libraries/openssl/pem.h"
#include "../../libraries/openssl/err.h"
#include "../../libraries/openssl/bio.h"
#include "../../libraries/openssl/evp.h"
#include "../../libraries/openssl/rand.h"
#include "../../libraries/openssl/aes.h"

#define KEY_LENGTH 4096
#define PADDING RSA_PKCS1_PADDING
#define AES_KEY_LENGTH 128

typedef struct {
    char *public_key_str;
    char *private_key_str;
    int (*keygen)(void); // 0 - if all was good | -1 - error case
    char *(*encrypt)(char *public_key_str, const char *data); // returns encrypted string
    char *(*decrypt)(char *encrypted_base64); // return decrypted string
    char *(*encrypt_text)(const char *text, const char *secret); // returns encrypted string
    char *(*decrypt_text)(const char *encrypted, const char *secret); // return decrypted string
    char *(*generate_random_key)(void); // return key string
    char *(*encrypt_data_for_db)(char *public_key_str, const char *text); // returns key encrypted (RSA) and text encrypted string, which separated with '\n'
    char *(*decrypt_data_from_db)(const char *text); // return decrypted string
    int (*verify_key_pair)(void); // returns 1 in good case, 0 - in error cases
} t_crypto;

int keygen(void);
char *encrypt(char *public_key_str, const char *data);
char *decrypt(char *encrypted_base64);
char *encrypt_text(const char* text, const char* secret);
char *decrypt_text(const char* encrypted, const char* secret);
char *generate_random_key(void);
int verify_key_pair(void);
char *encrypt_data_for_db(char *public_key_str, const char *text);
char *decrypt_data_from_db(const char *text);

t_crypto init_crypto(void);

#endif //CRYPTO_H
