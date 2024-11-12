#include "../../../inc/header.h"
#include "../../../inc/utils.h"

void register_rout(SSL *ssl, const char *request) {
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
    cJSON *username_item = cJSON_GetObjectItem(json, "name");
    cJSON *public_key_item = cJSON_GetObjectItem(json, "public_key");

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
    char *public_key = public_key_item->valuestring;

    /*if(public_key_item && cJSON_IsString(public_key_item) && is_valid_public_key(public_key_item->valuestring)){
        public_key = public_key_item->valuestring;
    }
    else {
        cJSON_AddStringToObject(response_json, "message", "Invalid public key");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }*/

    // Validate input
    if (!is_valid_login(user_login)) {
        cJSON_AddStringToObject(response_json, "message", "Login must be at least 5 characters long and contain only letters and digits.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    if (!is_valid_password(password)) {
        cJSON_AddStringToObject(response_json, "message", "Password must be at least 8 characters long, contain at least one letter, one digit, and one special character.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    // Connect to the database
    PGconn *conn = vendor.database.pool.acquire_connection();
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        cJSON_AddStringToObject(response_json, "message", "Database connection failed");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    PGresult *res = get_user_by_login(conn, user_login);
    if (res != NULL) {
        PQclear(res);
        cJSON_AddStringToObject(response_json, "message", "The login already exists. Please choose another one.");
        vendor.server.https.send_https_response(ssl, "409 Conflict", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }

    // username validation
    char *username_validation_result = username_validation(username_item ? username_item->valuestring : NULL, user_login, conn);

    if (!username_validation_result) {
        cJSON_AddStringToObject(response_json, "message", "Username must be at least 3 characters long and contain only letters, digits, and underscores. Try to choose another one");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    char *username = username_validation_result;
    char *password_hash = hash_password(password);

    // Insert user into the database
    int user_id = create_user(conn, username, user_login, password_hash, "public key", "en_US");
    if (user_id < 0) {
        cJSON_AddStringToObject(response_json, "message", "Unable to register user");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        free(password_hash);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }

    // Create response JSON with user details
    cJSON_AddStringToObject(response_json, "status", "success");
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


    free(token);
    PQclear(res);
    cJSON_Delete(json);
    cJSON_Delete(response_json);
    free(password_hash);
    vendor.database.pool.release_connection(conn);
}

