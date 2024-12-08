#include "../../../inc/header.h"
#include "../../../inc/utils.h"

void login_rout(SSL *ssl, const char *request) {
    char *body = strstr(request, "\r\n\r\n");
    cJSON *response_json = cJSON_CreateObject();

    if (!body) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "NO_MESSAGE_BODY");
        cJSON_AddStringToObject(response_json, "message", "No message body found in the request.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    body += 4;
    cJSON *json = cJSON_Parse(body);

    if (!json) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_JSON");
        cJSON_AddStringToObject(response_json, "message", "The provided JSON is invalid.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    cJSON *login_item = cJSON_GetObjectItem(json, "login");
    cJSON *password_item = cJSON_GetObjectItem(json, "password");

    if (!cJSON_IsString(login_item) || !cJSON_IsString(password_item)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "MISSING_CREDENTIALS");
        cJSON_AddStringToObject(response_json, "message", "Login and password are required.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    // Extract values
    char *user_login = login_item->valuestring;
    char *password = password_item->valuestring;

    if (strlen(user_login) < 5 || strlen(password) < 5) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_CREDENTIALS");
        cJSON_AddStringToObject(response_json, "message", "Login or password is too short.");
        vendor.server.https.send_https_response(ssl, "401 Unauthorized", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    PGconn *conn = vendor.database.pool.acquire_connection();
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "DB_CONNECTION_ERROR");
        cJSON_AddStringToObject(response_json, "message", "Failed to connect to the database.");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    PGresult *res = get_user_by_login(conn, user_login);
    if (res == NULL || PQntuples(res) == 0) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "USER_NOT_FOUND");
        cJSON_AddStringToObject(response_json, "message", "Invalid login or password.");
        vendor.server.https.send_https_response(ssl, "401 Unauthorized", "application/json", cJSON_Print(response_json));
        if (res) PQclear(res);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }

    char *stored_password_hash = PQgetvalue(res, 0, 3);
    char *password_hash = hash_password(password);
    if (strcmp(stored_password_hash, password_hash) != 0) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_PASSWORD");
        cJSON_AddStringToObject(response_json, "message", "Invalid login or password.");
        vendor.server.https.send_https_response(ssl, "401 Unauthorized", "application/json", cJSON_Print(response_json));
        PQclear(res);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        free(password_hash);
        vendor.database.pool.release_connection(conn);
        return;
    }

    // Success response
    cJSON_AddBoolToObject(response_json, "error", false);
    cJSON_AddStringToObject(response_json, "code", "SUCCESS");
    cJSON_AddStringToObject(response_json, "message", "User logged in successfully.");

    cJSON *user_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(user_json, "id", atoi(PQgetvalue(res, 0, 0)));
    cJSON_AddStringToObject(user_json, "name", PQgetvalue(res, 0, 1));
    cJSON_AddStringToObject(user_json, "login", PQgetvalue(res, 0, 2));
    cJSON_AddItemToObject(response_json, "user", user_json);

    cJSON *jwt_payload = cJSON_CreateObject();
    cJSON_AddNumberToObject(jwt_payload, "id", atoi(PQgetvalue(res, 0, 0)));
    cJSON_AddStringToObject(jwt_payload, "name", PQgetvalue(res, 0, 1));
    cJSON_AddStringToObject(jwt_payload, "login", PQgetvalue(res, 0, 2));

    char *token = _generate_jwt_token(jwt_payload);
    cJSON_AddStringToObject(response_json, "token", token);

    vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));

    if (token) free(token);
    if (res) PQclear(res);
    if (json) cJSON_Delete(json);
    if (response_json) cJSON_Delete(response_json);
    if (password_hash) free(password_hash);
    vendor.database.pool.release_connection(conn);
}
