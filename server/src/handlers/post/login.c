#include "../../../inc/header.h"
#include "../../../inc/utils.h"

void login_rout(SSL *ssl, const char *request) {
    char *body = strstr(request, "\r\n\r\n");
    cJSON *response_json = cJSON_CreateObject();

    if (!body) {
        cJSON_AddStringToObject(response_json, "message", "No message body");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    body += 4;
    cJSON *json = cJSON_Parse(body);

    if (!json) {
        cJSON_AddStringToObject(response_json, "message", "Invalid JSON");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    cJSON *login_item = cJSON_GetObjectItem(json, "login");
    cJSON *password_item = cJSON_GetObjectItem(json, "password");

    if (!cJSON_IsString(login_item) || !cJSON_IsString(password_item)) {
        cJSON_AddStringToObject(response_json, "message", "Login and password are required");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    // Extract values
    char *user_login = login_item->valuestring;
    char *password = password_item->valuestring;

    // Validate input (minimum length for login and password)
    if (strlen(user_login) < 5 || strlen(password) < 5) {
        cJSON_AddStringToObject(response_json, "message", "Invalid login or password.");
        vendor.server.https.send_https_response(ssl, "401 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    // Connect to the database
    PGconn *conn = vendor.database.pool.acquire_connection();
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        cJSON_AddStringToObject(response_json, "message", "Database connection failed");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json",
                                                cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    // Get user by login
    PGresult *res = get_user_by_login(conn, user_login);
    if (res == NULL || PQntuples(res) == 0) {
        cJSON_AddStringToObject(response_json, "message", "Invalid login or password.");
        vendor.server.https.send_https_response(ssl, "401 Unauthorized", "application/json",
                                                cJSON_Print(response_json));
        if (res) PQclear(res);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }

    // Verify password
    char *stored_password_hash = PQgetvalue(res, 0, 3);
    char *stored_password_salt = PQgetvalue(res, 0, 13);
    char *password_hash = hash_password(password, stored_password_salt);

    if (timing_safe_compare(stored_password_hash, password_hash, strlen(stored_password_hash)) != 0) {
        cJSON_AddStringToObject(response_json, "message", "Invalid login or password.");
        vendor.server.https.send_https_response(ssl, "401 Unauthorized", "application/json",
                                                cJSON_Print(response_json));
        PQclear(res);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        free(password_hash);
        PQfinish(conn);
        return;
    }

    // Create response JSON with user details
    cJSON_AddStringToObject(response_json, "status", "success");
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

    free(token);
    PQclear(res);
    cJSON_Delete(json);
    cJSON_Delete(response_json);
    free(password_hash);
    vendor.database.pool.release_connection(conn);
}
