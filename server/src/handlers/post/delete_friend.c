#include "../../../inc/handlers/post_handlers/delete_friend.h"


void delete_friend_rout(SSL *ssl, const char *request) {

    char *body = strstr(request, "\r\n\r\n");
    cJSON *response_json = cJSON_CreateObject();
    cJSON *json = NULL;
    PGconn *conn = NULL;
    PGresult *existing_chat = NULL;

    // Check if request body exists
    if (!body) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "NO_BODY");
        cJSON_AddStringToObject(response_json, "message", "No message body");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    body += 4;
    json = cJSON_Parse(body);

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
        cJSON_Delete(json);
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
    conn = vendor.database.pool.acquire_connection();
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Database connection failed: %s\n", PQerrorMessage(conn));
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "DB_CONNECTION_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Database connection failed");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
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
    PQclear(recipient_db_res);

    // Check if a personal chat exists
    existing_chat = get_personal_chat(conn, sender_id, recipient_id);
    if (!existing_chat || PQntuples(existing_chat) == 0) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "CHAT_NOT_FOUND");
        cJSON_AddStringToObject(response_json, "message", "No personal chat exists between the users");
        vendor.server.https.send_https_response(ssl, "404 Not Found", "application/json", cJSON_Print(response_json));
        if (existing_chat) PQclear(existing_chat);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }
    int chat_id = atoi(PQgetvalue(existing_chat, 0, 0));

    // Delete the personal chat
    if (!delete_personal_chat(conn, sender_id, recipient_id)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "DELETE_CHAT_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Failed to delete personal chat");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }

    if (is_user_online(recipient_id)) {
        cJSON *ws_message = cJSON_CreateObject();
        cJSON_AddNumberToObject(ws_message, "sender_id", sender_id);
        cJSON_AddStringToObject(ws_message, "message", "Delete friend");
        cJSON_AddNumberToObject(ws_message, "chat_id", chat_id);
        char *timestamp = NULL;
        get_current_timestamp(&timestamp);
        cJSON_AddStringToObject(ws_message, "timestamp", timestamp);
        _send_message_to_client(recipient_id, ws_message);
        if (ws_message) cJSON_Delete(ws_message);
    }

    // Respond with success
    cJSON_AddBoolToObject(response_json, "error", false);
    cJSON_AddStringToObject(response_json, "message", "Personal chat deleted successfully");
    vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));

    // Cleanup
    if (json) cJSON_Delete(json);
    if (response_json) cJSON_Delete(response_json);
    if (conn) vendor.database.pool.release_connection(conn);
    if (existing_chat) PQclear(existing_chat);
    return;
}

void protected_delete_friend_rout(SSL *ssl, const char *request) {
    jwt_middleware(ssl, request, delete_friend_rout);
}





