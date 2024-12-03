#include "../../../inc/header.h"
#include "../../../inc/utils.h"

void register_rout(SSL *ssl, const char *request) {
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
    cJSON *username_item = cJSON_GetObjectItem(json, "name");
    cJSON *public_key_item = cJSON_GetObjectItem(json, "public_key");

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
    char *public_key = public_key_item ? public_key_item->valuestring : NULL;

    // Validate login
    if (!is_valid_login(user_login)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_LOGIN");
        cJSON_AddStringToObject(response_json, "message", "Login must be at least 5 characters long and contain only letters and digits.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    // Validate password
    if (!is_valid_password(password)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_PASSWORD");
        cJSON_AddStringToObject(response_json, "message", "Password must be at least 8 characters long, contain at least one letter, one digit, and one special character.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    // Connect to the database
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

    // Check if login already exists
    PGresult *res = get_user_by_login(conn, user_login);
    if (res != NULL) {
        PQclear(res);
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "LOGIN_EXISTS");
        cJSON_AddStringToObject(response_json, "message", "The login already exists. Please choose another one.");
        vendor.server.https.send_https_response(ssl, "409 Conflict", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }

    // Validate username
    char *username_validation_result = username_validation(username_item ? username_item->valuestring : NULL, user_login, conn);
    if (!username_validation_result) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_USERNAME");
        cJSON_AddStringToObject(response_json, "message", "Username must be at least 3 characters long and contain only letters, digits, and underscores.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }

    char *username = username_validation_result;
    char *password_hash = hash_password(password);

    // Insert user into the database
    int user_id = create_user(conn, username, user_login, password_hash, public_key, "en_US");
    if (user_id < 0) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "USER_CREATION_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Unable to register user.");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        free(password_hash);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }

    // Success response
    cJSON_AddBoolToObject(response_json, "error", false);
    cJSON_AddStringToObject(response_json, "code", "SUCCESS");
    cJSON_AddStringToObject(response_json, "message", "User registered successfully.");

    cJSON *user_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(user_json, "id", user_id);
    cJSON_AddStringToObject(user_json, "name", username);
    cJSON_AddStringToObject(user_json, "login", user_login);
    cJSON_AddStringToObject(user_json, "public_key", public_key);
    cJSON_AddItemToObject(response_json, "user", user_json);

    cJSON *jwt_payload = cJSON_CreateObject();
    cJSON_AddNumberToObject(jwt_payload, "id", user_id);
    cJSON_AddStringToObject(jwt_payload, "name", username);
    cJSON_AddStringToObject(jwt_payload, "login", user_login);

    char *token = _generate_jwt_token(jwt_payload);
    cJSON_AddStringToObject(response_json, "token", token);

    vendor.server.https.send_https_response(ssl, "201 Created", "application/json", cJSON_Print(response_json));

    // Cleanup
    free(token);
    cJSON_Delete(json);
    cJSON_Delete(response_json);
    free(password_hash);
    vendor.database.pool.release_connection(conn);
}
