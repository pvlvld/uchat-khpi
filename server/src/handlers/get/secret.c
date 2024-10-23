#include "../../../inc/header.h"

void secret_rout(SSL *ssl, const char *request) {
    char *token = extract_bearer_token(request);

    cJSON *response_json = cJSON_CreateObject();

    jwt_verification_result result = vendor.jwt.verify_jwt_token(token);
    cJSON_AddItemToObject(response_json, "jwt_payload", result.payload);
    cJSON_AddNumberToObject(response_json, "status", result.status);

    char *response_body = cJSON_Print(response_json);
    vendor.server.https.send_https_response(ssl, "200 OK", "application/json", response_body);

    free(response_body);
}
