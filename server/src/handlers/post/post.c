#include "../../../inc/header.h"

void post_rout(SSL *ssl, const char *request) {
    char *token = extract_bearer_token(request);

    char *body = strstr(request, "\r\n\r\n");
    cJSON *response_json = cJSON_CreateObject();

    if (!body) {
        cJSON_AddStringToObject(response_json, "message", "No message body");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        free(token);
        return;
    }

    body += 4; // Skip the empty string
    cJSON *json = cJSON_Parse(body);

    if (!json) {
        cJSON_AddStringToObject(response_json, "message", "Invalid JSON");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        free(token);
        return;
    }

    jwt_verification_result result = vendor.jwt.verify_jwt_token(token);
    cJSON_AddItemToObject(response_json, "jwt_payload", result.payload);
    cJSON_AddNumberToObject(response_json, "status", result.status);

    char *response_body = cJSON_Print(response_json);
    vendor.server.https.send_https_response(ssl, "200 OK", "application/json", response_body);

    free(response_body);
    cJSON_Delete(json);

    cJSON_Delete(response_json);
    free(token);
}
