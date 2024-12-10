#include "../../../../../libraries/cJSON/cJSON.h"
#include "../../../../inc/utils.h"

char *get_sender_login_from_token(const char *request) {
    char *token = extract_bearer_token(request);
    if (!token) return NULL;

    // Verify and decode the JWT payload
    jwt_verification_result jwt_verify = vendor.jwt.verify_jwt_token(token);
    free(token); // Free the token after verification

    if (!jwt_verify.status) {
        return NULL; // Invalid or expired token
    }

    // Extract the `login` field from the JWT payload
    cJSON *login_item = cJSON_GetObjectItem(jwt_verify.payload, "login");
    if (!cJSON_IsString(login_item)) {
        cJSON_Delete(jwt_verify.payload);
        return NULL; // `login` field not found or invalid
    }

    char *login_name = strdup(login_item->valuestring);
    cJSON_Delete(jwt_verify.payload);
    return login_name; // Remember to free this in the calling function
}

char *get_sender_id_from_token(const char *request) {
    char *token = extract_bearer_token(request);
    if (!token) {
        return NULL;
    }

    // Verify and decode the JWT payload
    jwt_verification_result jwt_verify = vendor.jwt.verify_jwt_token(token);
    free(token); // Free the token after verification

    if (!jwt_verify.status) {
        return NULL; // Invalid or expired token
    }

    // Extract the `id` field from the JWT payload
    cJSON *user_id_item = cJSON_GetObjectItem(jwt_verify.payload, "id");
    if (!cJSON_IsNumber(user_id_item)) {
        cJSON_Delete(jwt_verify.payload);
        return NULL; // `user_id` field not found or invalid
    }

    printf("User ID: %d\n", user_id_item->valueint);
    char *user_id_str = malloc(10 * sizeof(char));
    itoa(user_id_item->valueint, user_id_str);
    cJSON_Delete(jwt_verify.payload);
    printf("User ID string: %s\n", user_id_str);
    return user_id_str; // Remember to free this in the calling function
}
