#include "../../inc/json_web_token.h"
#include "../../inc/header.h"

t_jwt init_jwt(void) {
    t_jwt jwt = {
        .generate_jwt_token = _generate_jwt_token,
        .verify_jwt_token = _verify_jwt_token,
        .helpers = {
            .base64url_encode = _base64url_encode,
            .base64url_decode = _base64url_decode,
            .sign_hmac_sha256 = _sign_hmac_sha256
        },
        .refresh_jwt_token = _refresh_jwt_token,
    };

    return jwt;
}

// Function for creating JWT token
char *_generate_jwt_token(cJSON *payload) {
    cJSON *header = cJSON_CreateObject();
    char *header_str = NULL;
    char *payload_str = NULL;
    char *encoded_header = NULL;
    char *encoded_payload = NULL;
    char data_to_sign[1024];
    unsigned int len;
    unsigned char *signature = NULL;
    char *encoded_signature = NULL;
    char *token = NULL;
    time_t now = time(NULL);
    char exp_str[32];

    if (!header || !payload) {
        cJSON_Delete(header);
        cJSON_Delete(payload);
        return NULL;
    }

    // Creating a header
    cJSON_AddStringToObject(header, "alg", "HS256");
    cJSON_AddStringToObject(header, "typ", "JWT");

    // Payload creation
    snprintf(exp_str, sizeof(exp_str), "%ld", now + 604800); // Convert time to a string
    cJSON_AddStringToObject(payload, "exp", exp_str); // Token expires in 1 hour

    // JSON serialization
    header_str = cJSON_PrintUnformatted(header);
    payload_str = cJSON_PrintUnformatted(payload);

    if (!header_str || !payload_str) {
        free(header_str);
        free(payload_str);
        cJSON_Delete(header);
        cJSON_Delete(payload);
        return NULL;
    }

    // Encoding in base64url
    encoded_header = vendor.jwt.helpers.base64url_encode((unsigned char*)header_str, strlen(header_str));
    encoded_payload = vendor.jwt.helpers.base64url_encode((unsigned char*)payload_str, strlen(payload_str));

    if (!encoded_header || !encoded_payload) {
        free(header_str);
        free(payload_str);
        free(encoded_header);
        free(encoded_payload);
        cJSON_Delete(header);
        cJSON_Delete(payload);
        return NULL;
    }

    // Forming a signature string
    snprintf(data_to_sign, sizeof(data_to_sign), "%s.%s", encoded_header, encoded_payload);

    // Creating a signature
    signature = vendor.jwt.helpers.sign_hmac_sha256(vendor.env.jwt_secret_key, data_to_sign, &len);
    encoded_signature = vendor.jwt.helpers.base64url_encode(signature, len);

    if (!encoded_signature) {
        free(header_str);
        free(payload_str);
        free(encoded_header);
        free(encoded_payload);
        free(signature);
        cJSON_Delete(header);
        cJSON_Delete(payload);
        return NULL;
    }

    // JWT Formation
    size_t token_len = strlen(encoded_header) + strlen(encoded_payload) + strlen(encoded_signature) + 3;
    token = malloc(token_len);
    if (token) {
        snprintf(token, token_len, "%s.%s.%s", encoded_header, encoded_payload, encoded_signature);
    }

    // Memory Release
    free(header_str);
    free(payload_str);
    free(encoded_header);
    free(encoded_payload);
   if (vendor.env.dev_mode) printf("signature: %s", signature);
    // free(signature);
    free(encoded_signature);
    cJSON_Delete(header);
    cJSON_Delete(payload);

    return token;
}

char *mx_strdup(const char *s1) {
    int length = strlen(s1);

    char *new_str = (char *)malloc((length + 1) * sizeof(char));
    if (!new_str)
        return NULL;

    memcpy(new_str, s1, length);

    new_str[length] = '\0';

    return new_str;
}

// Function for JWT decoding and signature verification
jwt_verification_result _verify_jwt_token(const char *token) {
    jwt_verification_result result = {0, NULL};
    char *token_copy = mx_strdup(token);
    if (!token_copy) return result;

    char *header_b64 = strtok(token_copy, ".");
    char *payload_b64 = strtok(NULL, ".");
    char *signature_b64 = strtok(NULL, ".");

    if (!header_b64 || !payload_b64 || !signature_b64) {
        free(token_copy);
        return result;
    }

    size_t header_len, payload_len, signature_len;
    unsigned char *decoded_header = vendor.jwt.helpers.base64url_decode(header_b64, &header_len);
    unsigned char *decoded_payload = vendor.jwt.helpers.base64url_decode(payload_b64, &payload_len);
    unsigned char *decoded_signature = vendor.jwt.helpers.base64url_decode(signature_b64, &signature_len);

    if (!decoded_header || !decoded_payload || !decoded_signature) {
        free(decoded_header);
        free(decoded_payload);
        free(decoded_signature);
        free(token_copy);
        return result;
    }

    cJSON *header = cJSON_Parse((const char *)decoded_header);
    cJSON *payload = cJSON_Parse((const char *)decoded_payload);

    if (!header || !payload) {
        free(decoded_header);
        free(decoded_payload);
        free(decoded_signature);
        cJSON_Delete(header);
        cJSON_Delete(payload);
        free(token_copy);
        return result;
    }

    // Check if the token has expired
    cJSON *exp_item = cJSON_GetObjectItem(payload, "exp");
    if (cJSON_IsString(exp_item)) {
        long exp = strtol(exp_item->valuestring, NULL, 10);
        if (time(NULL) >= exp) {
            cJSON_Delete(header);
            cJSON_Delete(payload);
            free(decoded_header);
            free(decoded_payload);
            free(decoded_signature);
            free(token_copy);
            return result;
        }
    }

    // Forming a signature string
    size_t data_to_sign_len = strlen(header_b64) + strlen(payload_b64) + 2;
    char *data_to_sign = malloc(data_to_sign_len);
    if (!data_to_sign) {
        cJSON_Delete(header);
        cJSON_Delete(payload);
        free(decoded_header);
        free(decoded_payload);
        free(decoded_signature);
        free(token_copy);
        return result;
    }

    snprintf(data_to_sign, data_to_sign_len, "%s.%s", header_b64, payload_b64);

    // Creating a signature
    unsigned int len;
    unsigned char *computed_signature = vendor.jwt.helpers.sign_hmac_sha256(vendor.env.jwt_secret_key, data_to_sign, &len);
    char *encoded_computed_signature = vendor.jwt.helpers.base64url_encode(computed_signature, len);

    // Signature Comparison
    int verified = strcmp((char *)encoded_computed_signature, signature_b64) == 0;

    if (verified) {
        // Check if “exp” and “phone_number” fields are present
        cJSON *exp_check = cJSON_GetObjectItem(payload, "exp");
        cJSON *phone_number_check = cJSON_GetObjectItem(payload, "phone_number");

        if (exp_check && phone_number_check) {
            result.status = 1;
            result.payload = cJSON_Duplicate(payload, 1);
        } else {
            result.status = 0;
        }
    }

    // Memory Release
    free(data_to_sign);
    cJSON_Delete(header);
    cJSON_Delete(payload);
    free(decoded_header);
    free(decoded_payload);
    free(decoded_signature);
    free(token_copy);
    free(encoded_computed_signature);

    return result;
}

// Function to update JWT token
char *_refresh_jwt_token(const char *old_token) {
    jwt_verification_result result = vendor.jwt.verify_jwt_token(old_token);
    if (!result.status) {
        // The old token is invalid
        return NULL;
    }

    // Create a new token based on the payload of the old one
    cJSON *new_payload = cJSON_Duplicate(result.payload, 1);
    char *new_token = vendor.jwt.generate_jwt_token(new_payload);

    // Memory Release
    cJSON_Delete(result.payload);

    return new_token;
}
