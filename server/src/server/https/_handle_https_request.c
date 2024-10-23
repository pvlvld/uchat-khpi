#include "../../../inc/server/https.h"
#include "../../../inc/header.h"

void _handle_https_request(SSL *ssl, const char *request) {
    if (strstr(request, "GET /hello") == request) {
        vendor.handlers.get._hello(ssl, request);
//        vendor.server.https.send_https_response(ssl, "200 OK", "application/json", "{\"message\": \"hello from https\"}");
        return;
    }
    if (strstr(request, "GET /secret") == request) {
        vendor.handlers.get.middleware._jwt(ssl, request, vendor.handlers.get.secret);
        return;
    }

    if (strstr(request, "POST /post") == request) {
        vendor.handlers.post.middleware._jwt(ssl, request, vendor.handlers.post.post);
        return;
    }
    if (strstr(request, "POST /login") == request) {
        vendor.handlers.post._login(ssl, request);
        return;
    }

    cJSON *response_json = cJSON_CreateObject();
    cJSON_AddStringToObject(response_json, "message", "Not found");
    vendor.server.https.send_https_response(ssl, "404 Not Found", "application/json", cJSON_Print(response_json));
    cJSON_Delete(response_json);
}
