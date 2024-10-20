#include "../../../inc/server/https.h"
#include "../../../inc/header.h"

void _handle_https_request(SSL *ssl, const char *request) {
    if (!request) printf("tets\n");
    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "message", "Page from https");
    vendor.server.https.send_https_response(ssl, "404 Not Found", "application/json", cJSON_Print(response_json));
    cJSON_Delete(response_json);
}
