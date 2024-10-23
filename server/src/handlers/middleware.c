#include "../../inc/header.h"

void jwt_middleware(SSL *ssl, const char *request, void (*next)(SSL *ssl, const char *request)) {
    char *token = extract_bearer_token(request);
    if (!token) {
        cJSON *response_json = cJSON_CreateObject();
        cJSON_AddStringToObject(response_json, "message", "Missing Authorization Header");
        vendor.server.https.send_https_response(ssl, "401 Unauthorized", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        if (vendor.env.dev_mode) printf("Missing Authorization Header\n");
        return;
    }

    if (!vendor.jwt.verify_jwt_token(token).status) {
        cJSON *response_json = cJSON_CreateObject();
        cJSON_AddStringToObject(response_json, "message", "Invalid or expired JWT token");
        vendor.server.https.send_https_response(ssl, "401 Unauthorized", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
    }

    next(ssl, request);
}
