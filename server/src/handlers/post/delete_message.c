#include "../../../inc/handlers/post_handlers/delete_message.h"

void delete_message_rout(SSL *ssl, const char *request) {
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
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }
    if (!is_user_in_chat(conn, chat_id, user_id)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "ACCESS_DENIED");
        cJSON_AddStringToObject(response_json, "message", "The sender has no access to this chat. Or chat ID is invalid.");
        vendor.server.https.send_https_response(ssl, "409 Conflict", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }
    const char *chat_type = get_chat_type(conn, chat_id);
    if (strcmp(chat_type, "group") == 0) {
        const char *user_role = get_user_role_in_group(conn, chat_id, sender_id);
        if (!user_role || strcmp(user_role, "banned") == 0 || strcmp(user_role, "restricted") == 0) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "FORBIDDEN");
            cJSON_AddStringToObject(response_json, "message", "User is not allowed to delete messages.");
            vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json", cJSON_Print(response_json));
            free(chat_id_str);
            free(message_id_str);
            free(user_id_str);
            cJSON_Delete(json);
            cJSON_Delete(response_json);
            PQfinish(conn);
            return;

        } else if (strcmp(user_role, "admin") == 0 || strcmp(user_role, "owner") == 0) {
            bool any_delete_allowed = true;
            deleteMessageResult_t result = delete_message_db_and_return_data(conn, chat_id, message_id, user_id, any_delete_allowed);
        } else if (strcmp(user_role, "member") == 0) {
            bool any_delete_allowed = false;
            deleteMessageResult_t result = delete_message_db_and_return_data(conn, chat_id, message_id, user_id, any_delete_allowed);
        } else {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_DELETION_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message deletion failed.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
            free(chat_id_str);
            free(message_id_str);
            free(user_id_str);
            cJSON_Delete(json);
            cJSON_Delete(response_json);
            PQfinish(conn);
            return;
        }
        if (result.Success == -1) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "FORBIDDEN");
            cJSON_AddStringToObject(response_json, "message", "User is not allowed to delete this message.");
            vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json", cJSON_Print(response_json));
        } else if (result.Success == 0) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_DB_DELETION_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message DB deletion failed.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        } else if (result.Success == 1) {
            cJSON_AddBoolToObject(response_json, "error", false);
            cJSON_AddStringToObject(response_json, "code", "OK");
            cJSON_AddStringToObject(response_json, "message", "Message deleted successfully");
            cJSON_AddNumberToObject(response_json, "message_id", result.message_id);
            cJSON_AddStringToObject(response_json, "timestamp", result.timestamp);
            vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));
        } else {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_DELETION_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message deletion failed.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        }
    } else if (strcmp(chat_type, "channel") == 0) {
        const char *user_role = get_user_role_in_group(conn, chat_id, sender_id);
        if (!user_role || strcmp(user_role, "admin") != 0 || strcmp(user_role, "owner") != 0) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "FORBIDDEN");
            cJSON_AddStringToObject(response_json, "message", "User is not allowed to delete messages.");
            vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json", cJSON_Print(response_json));
            free(chat_id_str);
            free(message_id_str);
            free(user_id_str);
            cJSON_Delete(json);
            cJSON_Delete(response_json);
            PQfinish(conn);
            return;

        } else if (strcmp(user_role, "admin") == 0 || strcmp(user_role, "owner") == 0) {
            bool any_delete_allowed = true;
            delete_message_db_and_return_data(conn, chat_id, message_id, user_id, any_delete_allowed);
        } else {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_DELETION_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message deletion failed.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
            free(chat_id_str);
            free(message_id_str);
            free(user_id_str);
            cJSON_Delete(json);
            cJSON_Delete(response_json);
            PQfinish(conn);
            return;
        }
        if (result.Success == -1) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "FORBIDDEN");
            cJSON_AddStringToObject(response_json, "message", "User is not allowed to delete this message.");
            vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json", cJSON_Print(response_json));
        } else if (result.Success == 0) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_DB_DELETION_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message DB deletion failed.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        } else if (result.Success == 1) {
            cJSON_AddBoolToObject(response_json, "error", false);
            cJSON_AddStringToObject(response_json, "code", "OK");
            cJSON_AddStringToObject(response_json, "message", "Message deleted successfully");
            cJSON_AddNumberToObject(response_json, "message_id", result.message_id);
            cJSON_AddStringToObject(response_json, "timestamp", result.timestamp);
            vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));
        } else {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_DELETION_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message deletion failed.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        }
    } else if (strcmp(chat_type, "personal") == 0) {
        deleteMessageResult_t result = delete_message_db_and_return_data(conn, chat_id, message_id, user_id);

        if (result.Success == -1) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "FORBIDDEN");
            cJSON_AddStringToObject(response_json, "message", "User is not allowed to delete this message.");
            vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json", cJSON_Print(response_json));
        } else if (result.Success == 0) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_DB_DELETION_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message DB deletion failed.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        } else if (result.Success == 1) {
            cJSON_AddBoolToObject(response_json, "error", false);
            cJSON_AddStringToObject(response_json, "code", "OK");
            cJSON_AddStringToObject(response_json, "message", "Message deleted successfully");
            cJSON_AddNumberToObject(response_json, "message_id", result.message_id);
            cJSON_AddStringToObject(response_json, "timestamp", result.timestamp);
            vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));
        } else {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_DELETION_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message deletion failed.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        }

    } else {
        // TODO: unknown chat type error response
    }

    free(chat_id_str);
    free(message_id_str);
    free(user_id_str);
    cJSON_Delete(json);
    cJSON_Delete(response_json);
    PQfinish(conn);
    return;

}

void protected_delete_message_rout(SSL *ssl, const char *request) { jwt_middleware(ssl, request, delete_message_rout); }



