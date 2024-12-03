#include "../../../../inc/handlers/post_handlers/edit_message.h"
bool send_ws_edit_message(PGconn *conn, int chat_id, int sender_id) {
    const char *chat_type = get_chat_type_ptr(conn, chat_id);
    if (strcmp(chat_type, "personal") == 0) {
        int recipient_id = get_dm_recipient_id(conn, chat_id, sender_id);
        if (!is_user_online(recipient_id)) { return 0; }
        cJSON *json_message = create_message_json(sender_id, "Edit message");
        _send_message_to_client(recipient_id, json_message);
        cJSON_Delete(json_message);
        return 1;
    } else if (strcmp(chat_type, "group") == 0) {
        PGresult *recipients = get_group_recipients(conn, chat_id);
        if (!recipients || PQresultStatus(recipients) != PGRES_TUPLES_OK) { return 0; }
        for (int i = 0; i < PQntuples(recipients); i++) {
            int recipient_id = atoi(PQgetvalue(recipients, i, 0));
            if (!is_user_online(recipient_id)) { continue; }
            cJSON *json_message = create_message_json(sender_id, "Edit message");
            _send_message_to_client(recipient_id, json_message);
            cJSON_Delete(json_message);
        }
        PQclear(recipients);
        return 1;
    } else if (strcmp(chat_type, "channel") == 0) {
        PGresult *recipients = get_group_recipients(conn, chat_id);
        if (!recipients || PQresultStatus(recipients) != PGRES_TUPLES_OK) { return 0; }
        for (int i = 0; i < PQntuples(recipients); i++) {
            int recipient_id = atoi(PQgetvalue(recipients, i, 0));
            if (!is_user_online(recipient_id)) { continue; }
            cJSON *json_message = create_message_json(sender_id, "Edit message");
            _send_message_to_client(recipient_id, json_message);
            cJSON_Delete(json_message);
        }
        PQclear(recipients);
        return 1;

    } else {
        return 0;
    }
}
void send_edit_message_response(SSL *ssl, EditMessageResult_t *edit_message_result, cJSON *response_json) {

    if (edit_message_result->Error == 1) {
        cJSON_AddBoolToObject(response_json, "error", true);

        // Map error codes to response details
        if (strcmp(edit_message_result->error_code, "UNKNOWN_CHAT_TYPE") == 0) {
            cJSON_AddStringToObject(response_json, "code", "UNKNOWN_CHAT_TYPE");
            cJSON_AddStringToObject(response_json, "message", "The chat type is unknown or unsupported.");
            vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json",
                                                    cJSON_Print(response_json));
        } else if (strcmp(edit_message_result->error_code, "USER_ROLE_NOT_FOUND") == 0) {
            cJSON_AddStringToObject(response_json, "code", "USER_ROLE_NOT_FOUND");
            cJSON_AddStringToObject(response_json, "message",
                                    "The user's role in the group or channel could not be determined.");
            vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json",
                                                    cJSON_Print(response_json));
        } else if (strcmp(edit_message_result->error_code, "MESSAGE_SENDER_NOT_FOUND") == 0) {
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_SENDER_NOT_FOUND");
            cJSON_AddStringToObject(response_json, "message", "The sender of the message could not be found.");
            vendor.server.https.send_https_response(ssl, "404 Not Found", "application/json",
                                                    cJSON_Print(response_json));
        } else if (strcmp(edit_message_result->error_code, "USER_NOT_ALLOWED_TO_EDIT_THIS_MESSAGE") == 0) {
            cJSON_AddStringToObject(response_json, "code", "FORBIDDEN");
            cJSON_AddStringToObject(response_json, "message", "The user is not allowed to edit this message.");
            vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json",
                                                    cJSON_Print(response_json));
        } else if (strcmp(edit_message_result->error_code, "EDIT_MESSAGE_DB_ERROR") == 0) {
            cJSON_AddStringToObject(response_json, "code", "DB_ERROR");
            cJSON_AddStringToObject(response_json, "message",
                                    "An error occurred while editing the message in the database.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json",
                                                    cJSON_Print(response_json));
        } else if (strcmp(edit_message_result->error_code, "TIMESTAMP_QUERY_ERROR") == 0) {
            cJSON_AddStringToObject(response_json, "code", "TIMESTAMP_ERROR");
            cJSON_AddStringToObject(response_json, "message", "An error occurred while retrieving the timestamp.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json",
                                                    cJSON_Print(response_json));
        } else if (strcmp(edit_message_result->error_code, "WS_ERROR") == 0) {
            cJSON_AddStringToObject(response_json, "code", "WEBSOCKET_ERROR");
            cJSON_AddStringToObject(response_json, "message", "An error occurred while sending WebSocket message/s.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json",
                                                    cJSON_Print(response_json));
        } else {
            cJSON_AddStringToObject(response_json, "code", "UNKNOWN_ERROR");
            cJSON_AddStringToObject(response_json, "message", "An unknown error occurred.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json",
                                                    cJSON_Print(response_json));
        }
    } else {
        cJSON_AddBoolToObject(response_json, "error", false);
        cJSON_AddStringToObject(response_json, "code", "SUCCESS");
        cJSON_AddStringToObject(response_json, "message", "Message edited successfully.");
        cJSON_AddNumberToObject(response_json, "message_id", edit_message_result->message_id);
        cJSON_AddStringToObject(response_json, "timestamp", edit_message_result->timestamp);
        vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));
    }

    // Cleanup
    cJSON_Delete(response_json);
}
