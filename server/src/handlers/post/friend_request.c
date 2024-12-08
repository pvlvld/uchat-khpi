#include "../../../inc/handlers/post_handlers/friend_request.h"

void friend_request_rout(SSL *ssl, const char *request) {
    char *body = strstr(request, "\r\n\r\n");
    cJSON *response_json = cJSON_CreateObject();

    // Check if request body exists
    if (!body) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "NO_BODY");
        cJSON_AddStringToObject(response_json, "message", "No message body");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    body += 4; // Skip headers
    cJSON *json = cJSON_Parse(body);

    // Check if JSON is valid
    if (!json) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_JSON");
        cJSON_AddStringToObject(response_json, "message", "Invalid JSON format");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    // Extract sender and recipient information
    char *recipient_username_str = extract_recipient_username(json);
    char *sender_id_str = get_sender_id_from_token(request); // Extracted from JWT

    printf("Sender ID: %s\n", sender_id_str);
    if (!recipient_username_str || !sender_id_str) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "MISSING_USER_IDS");
        cJSON_AddStringToObject(response_json, "message", "Sender ID or recipient username is missing");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }
    if (!is_valid_user_id(sender_id_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_SENDER_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid sender ID");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    if (!is_valid_username(recipient_username_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_USER_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid username");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    int sender_id = atoi(sender_id_str);

    // Connect to database
    PGconn *conn = vendor.database.pool.acquire_connection();
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Database connection failed: %s\n", PQerrorMessage(conn));
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "DB_CONNECTION_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Database connection failed");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }

    PGresult *sender_db_res = get_user_by_id(conn, sender_id);
    if (!sender_db_res || PQntuples(sender_db_res) == 0) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "USER_NOT_FOUND");
        cJSON_AddStringToObject(response_json, "message", "Sender ID not found");
        vendor.server.https.send_https_response(ssl, "404 Not Found", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        if (sender_db_res) PQclear(sender_db_res);
        return;
    }
    char *sender_username_str = PQgetvalue(sender_db_res, 0, 1);
    if (!is_valid_username(sender_username_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_USER_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid sender username");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    PGresult *recipient_db_res = get_user_by_username(conn, recipient_username_str);
    if (!recipient_db_res || PQntuples(recipient_db_res) == 0) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "USER_NOT_FOUND");
        cJSON_AddStringToObject(response_json, "message", "Recipient username not found");
        vendor.server.https.send_https_response(ssl, "404 Not Found", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        if (recipient_db_res) PQclear(recipient_db_res);
        return;
    }

    // Extract the recipient ID
    char *recipient_id_str = PQgetvalue(recipient_db_res, 0, 0);
    if (!is_valid_user_id(recipient_id_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_USER_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid user ID");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        PQclear(recipient_db_res);
        return;
    }

    int recipient_id = atoi(recipient_id_str);
    char *recipient_login = PQgetvalue(recipient_db_res, 0, 2);    // user_login
    char *recipient_about = PQgetvalue(recipient_db_res, 0, 3);    // about
    char *recipient_public_key = PQgetvalue(recipient_db_res, 0, 8); // public_key
    printf("Recipient login: %s\n", recipient_login);
    printf("Recipient about: %s\n", recipient_about);
    printf("Recipient public key: %s\n", recipient_public_key);
    if (!recipient_login || !recipient_about || !recipient_public_key) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "GET_USER_INFO_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Failed to get user data from database");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }

    printf("Sender ID: %d\n", sender_id);
    printf("Recipient ID: %d\n", recipient_id);

    // Check if sender and recipient are the same
    if (sender_id == recipient_id) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_USER_IDS");
        cJSON_AddStringToObject(response_json, "message", "Cannot create a personal chat with yourself");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }

    // Check if a personal chat already exists
    PGresult *existing_chat = get_personal_chat(conn, sender_id, recipient_id);
    if (existing_chat == NULL || PQntuples(existing_chat) != 0) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "CHAT_ALREADY_EXISTS");
        cJSON_AddStringToObject(response_json, "message", "Personal chat already exists");
        vendor.server.https.send_https_response(ssl, "409 Conflict", "application/json", cJSON_Print(response_json));
        PQclear(existing_chat);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }
    PQclear(existing_chat);

    // Create a new personal chat
    int chat_id = create_personal_chat(conn, sender_id, recipient_id);
    if (chat_id == -1) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "CHAT_CREATION_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Failed to create personal chat");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }

    if (is_user_online(recipient_id)) {
        cJSON *ws_message = cJSON_CreateObject();
        cJSON_AddNumberToObject(ws_message, "sender_id", sender_id);
        cJSON_AddStringToObject(ws_message, "sender_username", sender_username_str);
        cJSON_AddStringToObject(ws_message, "message", "Friend request");
        cJSON_AddNumberToObject(ws_message, "chat_id", chat_id);
        char *timestamp = NULL;
        get_current_timestamp(&timestamp);
        cJSON_AddStringToObject(ws_message, "timestamp", timestamp);
        if (recipient_login) cJSON_AddStringToObject(ws_message, "recipient_login", recipient_login);
        if (recipient_about) cJSON_AddStringToObject(ws_message, "recipient_about", recipient_about);
        if (recipient_public_key) cJSON_AddStringToObject(ws_message, "recipient_public_key", recipient_public_key);
        _send_message_to_client(recipient_id, ws_message);
        if (timestamp) free(timestamp);
        if (ws_message) cJSON_Delete(ws_message);
    }

    // Respond with success
    cJSON_AddBoolToObject(response_json, "error", false);
    cJSON_AddStringToObject(response_json, "message", "Personal chat created successfully");
    cJSON_AddNumberToObject(response_json, "chat_id", chat_id);
    cJSON_AddNumberToObject(response_json, "recipient_id", recipient_id);
    char *timestamp = NULL;
    get_current_timestamp(&timestamp);
    cJSON_AddStringToObject(response_json, "timestamp", timestamp);
    if (recipient_login) cJSON_AddStringToObject(response_json, "recipient_login", recipient_login);
    if (recipient_about) cJSON_AddStringToObject(response_json, "recipient_about", recipient_about);
    if (recipient_public_key) cJSON_AddStringToObject(response_json, "recipient_public_key", recipient_public_key);
    vendor.server.https.send_https_response(ssl, "201 Created", "application/json", cJSON_Print(response_json));

    // Cleanup
    if (json) cJSON_Delete(json);
    if (response_json) cJSON_Delete(response_json);
    if (conn) PQfinish(conn);
    if (recipient_db_res) PQclear(recipient_db_res);
    if (sender_db_res) PQclear(sender_db_res);
    //if (existing_chat) PQclear(existing_chat);
    return;
}

void protected_friend_request_rout(SSL *ssl, const char *request) {
    jwt_middleware(ssl, request, friend_request_rout);
}


