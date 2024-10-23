#include "../../../inc/header.h"

void login_rout(SSL *ssl, const char *request) {
    char *body = strstr(request, "\r\n\r\n");
    cJSON *response_json = cJSON_CreateObject();

    if (!body) {
        cJSON_AddStringToObject(response_json, "message", "No message body");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    body += 4; // Skip the empty string
    cJSON *json = cJSON_Parse(body);

    if (!json) {
        cJSON_AddStringToObject(response_json, "message", "Invalid JSON");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    cJSON *login_item = cJSON_GetObjectItem(json, "login");
    cJSON *password_item = cJSON_GetObjectItem(json, "password");

    cJSON_AddStringToObject(response_json, "login", login_item->valuestring);
    cJSON_AddStringToObject(response_json, "password", password_item->valuestring);

    char *response_body = cJSON_Print(response_json);
    vendor.server.https.send_https_response(ssl, "200 OK", "application/json", response_body);

    free(response_body);
    cJSON_Delete(json);

    cJSON_Delete(response_json);
}
