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
    char *recipient_id_str = extract_user_id(json);
    char *sender_id_str = get_sender_id_from_token(request); // Extracted from JWT

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
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    if (!is_valid_user_id(recipient_id_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_USER_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid user ID");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    int sender_id = atoi(sender_id_str);
    int recipient_id = atoi(recipient_id_str);

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
        PQfinish(conn);
        return;
    }
    PQclear(existing_chat);

    // Delete the personal chat
    if (!delete_personal_chat(conn, sender_id, recipient_id)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "DELETE_CHAT_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Failed to delete personal chat");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }

    // Respond with success
    cJSON_AddBoolToObject(response_json, "error", false);
    cJSON_AddStringToObject(response_json, "message", "Personal chat deleted successfully");
    vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));

    // Cleanup
    if (json) cJSON_Delete(json);
    if (response_json) cJSON_Delete(response_json);
    if (conn) PQfinish(conn);
    return;
}

void protected_delete_friend_rout(SSL *ssl, const char *request) {
    jwt_middleware(ssl, request, delete_friend_rout);
}





