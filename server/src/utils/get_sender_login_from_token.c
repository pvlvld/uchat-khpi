#include <../../../../libraries/cJSON/cJSON.h>
#include "../../../inc/utils.h"

char *get_sender_from_token(const char *request) {
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

