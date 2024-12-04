#include "../../../inc/handlers/post_handlers/edit_message.h"


void edit_message_rout(SSL *ssl, const char *request) {
    char *body = strstr(request, "\r\n\r\n");
    cJSON *response_json = cJSON_CreateObject();

    if (!body) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "NO_BODY");
        cJSON_AddStringToObject(response_json, "message", "No message body");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    body += 4;
    cJSON *json = cJSON_Parse(body);

    if (!json) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_JSON");
        cJSON_AddStringToObject(response_json, "message", "Invalid JSON format");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    char *chat_id_str = extract_chat_id(json);
    char *message_id_str = extract_message_id(json);
    char *new_message_text_str = extract_message(json);
    char *user_id_str = get_sender_id_from_token(request);

    if (!is_valid_chat_id(chat_id_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_CHAT_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid chat ID");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    if (!is_valid_message_id(message_id_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_MESSAGE_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid message ID");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    if (!is_valid_user_id(user_id_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_USER_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid user ID");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    int chat_id = atoi(chat_id_str);
    int message_id = atoi(message_id_str);
    int user_id = atoi(user_id_str);

    // Connect to the database
    PGconn *conn = vendor.database.pool.acquire_connection();
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "DB_CONNECTION_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Database connection failed");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json",
                                                cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }

    if (!is_user_in_chat(conn, chat_id, user_id)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "ACCESS_DENIED");
        cJSON_AddStringToObject(response_json, "message",
                                "The sender has no access to this chat. Or chat ID is invalid.");
        vendor.server.https.send_https_response(ssl, "409 Conflict", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }

    EditMessageResult_t edit_message_result = perform_message_edit(conn, chat_id, user_id, message_id, new_message_text_str);

    bool ws_edit_message_res = true;
    if (edit_message_result.Error != 1) ws_edit_message_res = send_ws_edit_message(conn, chat_id, user_id, edit_message_result);
    if (ws_edit_message_res == false) {
            edit_message_result.error_code = "WS_ERROR";
            edit_message_result.Error = 1;
    }
    send_edit_message_response(ssl, &edit_message_result, response_json);

    if (chat_id_str) free(chat_id_str);
    if (message_id_str) free(message_id_str);
    if (user_id_str) free(user_id_str);
    if (new_message_text_str) free(new_message_text_str);
    if (json) cJSON_Delete(json);
    if (edit_message_result.timestamp != NULL) {
        free(edit_message_result.timestamp);
    }

    PQfinish(conn);
    return;
}

void protected_edit_message_rout(SSL *ssl, const char *request) { jwt_middleware(ssl, request, edit_message_rout); }


















