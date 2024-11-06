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
    char *username = username_item && cJSON_IsString(username_item) ? username_item->valuestring : user_login;

    // Validate input (minimum length for login and password)
    if (strlen(user_login) < 5 || strlen(password) < 5) {
        cJSON_AddStringToObject(response_json, "message", "Login and password must be at least 5 characters long");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    // Connect to the database
    const char *conninfo = "dbname=your_dbname user=your_username password=your_password hostaddr=127.0.0.1 port=5432";

    PGconn *conn = PQconnectdb(conninfo);
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
    cJSON_AddItemToObject(response_json, "user", user_json);

    char *response_body = cJSON_Print(response_json);
    vendor.server.https.send_https_response(ssl, "201 Created", "application/json", response_body);

    PQclear(res);
    free(response_body);
    cJSON_Delete(json);
    cJSON_Delete(response_json);
    free(password_hash);
    PQfinish(conn);
}

