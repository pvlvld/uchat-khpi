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
    char *recipient_id_str = extract_user_id(json);
    char *sender_id_str = get_sender_id_from_token(request); // Extracted from JWT

    printf("Sender ID: %s\n", sender_id_str);
    if (!recipient_id_str || !sender_id_str) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "MISSING_USER_IDS");
        cJSON_AddStringToObject(response_json, "message", "Sender or recipient ID is missing");
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

    if (!is_valid_user_id(recipient_id_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_USER_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid user ID");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    int sender_id = atoi(sender_id_str);
    int recipient_id = atoi(recipient_id_str);

    printf("Sender ID: %d\n", sender_id);
    printf("Recipient ID: %d\n", recipient_id);
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
        cJSON_AddStringToObject(ws_message, "message", "Friend request");
        cJSON_AddNumberToObject(ws_message, "chat_id", chat_id);
        char *timestamp = NULL;
        get_current_timestamp(&timestamp);
        cJSON_AddStringToObject(ws_message, "timestamp", timestamp);
        _send_message_to_client(recipient_id, ws_message);
        if (ws_message) cJSON_Delete(ws_message);
    }

    // Respond with success
    cJSON_AddBoolToObject(response_json, "error", false);
    cJSON_AddStringToObject(response_json, "message", "Personal chat created successfully");
    cJSON_AddNumberToObject(response_json, "chat_id", chat_id);
    vendor.server.https.send_https_response(ssl, "201 Created", "application/json", cJSON_Print(response_json));

    // Cleanup
    if (json) cJSON_Delete(json);
    if (response_json) cJSON_Delete(response_json);
    if (conn) PQfinish(conn);
    //if (existing_chat) PQclear(existing_chat);
    return;
}

void protected_friend_request_rout(SSL *ssl, const char *request) {
    jwt_middleware(ssl, request, friend_request_rout);
}


