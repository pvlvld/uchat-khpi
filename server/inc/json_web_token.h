#ifndef JSON_WEB_TOKEN_H
#define JSON_WEB_TOKEN_H

#include "../../libraries/cJSON/build/include/cjson/cJSON.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    int status;       // Validation status (0 - invalid, 1 - valid)
    cJSON *payload;      // User Login
} jwt_verification_result;

typedef struct {
    char *(*base64url_encode)(const unsigned char *data, size_t length);
    unsigned char *(*base64url_decode)(const char *input, size_t *length);
    unsigned char *(*sign_hmac_sha256)(const char *key, const char *data, unsigned int *len);
} t_jwt_helpers;

typedef struct {
    char *(*generate_jwt_token)(cJSON *jwt_body);
    jwt_verification_result (*verify_jwt_token)(const char *token);
    char *(*refresh_jwt_token)(const char *old_token);
    t_jwt_helpers helpers;
} t_jwt;
char *_generate_jwt_token(cJSON *jwt_body);
jwt_verification_result _verify_jwt_token(const char *token);
t_jwt init_jwt(void);

char *_base64url_encode(const unsigned char *data, size_t length);
unsigned char *_base64url_decode(const char *input, size_t *length);
unsigned char *_sign_hmac_sha256(const char *key, const char *data, unsigned int *len);
char *_refresh_jwt_token(const char *old_token);
char *extract_bearer_token(const char *request);

#endif //JSON_WEB_TOKEN_H
