#include "../../../inc/header.h"
#include "../../../inc/utils.h"

void send_message_rout(SSL *ssl, const char *request) {
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
    char *message_str = extract_message(json); // TODO: Validate message for supported types and potentially return an object
    char *sender_login_str = get_sender_login_from_token(request);

    if (!is_valid_chat_id(chat_id_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_CHAT_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid chat ID");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));

        cJSON_Delete(response_json);
        free(message_str);
        free(chat_id_str);
        free(sender_login_str);
        return;
    }

    if (!is_valid_login(sender_login_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_SENDER_LOGIN");
        cJSON_AddStringToObject(response_json, "message", "Invalid sender login.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));

        cJSON_Delete(response_json);
        free(message_str);
        free(chat_id_str);
        free(sender_login_str);
        return;
    }

    //printf("message = %s \n", message_str);
    /*if (!is_valid_message(message_str)) { // TODO: check validity for every message type we support
        cJSON_AddStringToObject(
            response_json, "message",
            "Invalid message."); // 1 - 5000 characters, only letters, numbers and special characters for now
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));

        cJSON_Delete(response_json);
        free(message_str);
        free(chat_id_str);
        free(sender_login_str);
        return;
    }*/

    int chat_id = atoi(chat_id_str);
    free(chat_id_str);

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
        vendor.database.pool.release_connection(conn);
        return;
    }

    PGresult *res = get_user_by_login(conn, sender_login_str);
    if (res == NULL) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "USER_NOT_FOUND");
        cJSON_AddStringToObject(response_json, "message", "The login, received from the token, does not exist.");
        vendor.server.https.send_https_response(ssl, "409 Conflict", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }

    int sender_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    // Check if user is part of the chat
    if (!is_user_in_chat(conn, chat_id, sender_id)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "ACCESS_DENIED");
        cJSON_AddStringToObject(response_json, "message", "The sender has no access to this chat. Or chat ID is invalid.");
        vendor.server.https.send_https_response(ssl, "409 Conflict", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }

    const char *chat_type = get_chat_type_ptr(conn, chat_id);
    printf("Chat type: %s\n", chat_type);
    if (strcmp(chat_type, "group") == 0) {
        // For group chats, check user permissions
        const char *user_role = get_user_role_in_group(conn, chat_id, sender_id);
        if (!user_role || strcmp(user_role, "banned") == 0 || strcmp(user_role, "restricted") == 0) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "FORBIDDEN");
            cJSON_AddStringToObject(response_json, "message", "User is not allowed to write in this group.");
            vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json", cJSON_Print(response_json));
            cJSON_Delete(json);
            cJSON_Delete(response_json);
            vendor.database.pool.release_connection(conn);
            return;
        }

        MessageResult_t group_chat_message_res = handle_group_or_channel_message(conn, chat_id, sender_id, message_str,
                                                                     0, 0, 0, 0, 0);

        if (group_chat_message_res.Success== 1) {
            cJSON_AddBoolToObject(response_json, "error", false);
            cJSON_AddStringToObject(response_json, "status", "success");
            cJSON_AddStringToObject(response_json, "message", "Message sent successfully.");
            cJSON_AddNumberToObject(response_json, "message_id", group_chat_message_res.message_id);
            cJSON_AddStringToObject(response_json, "timestamp", group_chat_message_res.timestamp);
            vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));
        } else if (group_chat_message_res.Success== -1) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "DB_INSERT_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message failed to be added to the database.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        } else if (group_chat_message_res.Success == -2) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "RECIPIENTS_NOT_FOUND");
            cJSON_AddStringToObject(response_json, "message", "Failed to find group recipients.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        } else {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_SEND_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message failed to be sent.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        }
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);

    } else if (strcmp(chat_type, "channel") == 0) {
        // For channels, ensure the user is the owner
        const char *user_role = get_user_role_in_group(conn, chat_id, sender_id);
        if (!user_role || strcmp(user_role, "owner") != 0) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "FORBIDDEN");
            cJSON_AddStringToObject(response_json, "message", "User is not allowed to write in this channel.");
            vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json", cJSON_Print(response_json));
            cJSON_Delete(json);
            cJSON_Delete(response_json);
            vendor.database.pool.release_connection(conn);
            return;
        }

        MessageResult_t channel_chat_message_res = handle_group_or_channel_message(conn, chat_id, sender_id, message_str,
                                                                       0, 0, 0, 0, 0);

        if (channel_chat_message_res.Success == 1) {
            cJSON_AddBoolToObject(response_json, "error", false);
            cJSON_AddStringToObject(response_json, "status", "success");
            cJSON_AddStringToObject(response_json, "message", "Message sent successfully.");
            cJSON_AddNumberToObject(response_json, "message_id", channel_chat_message_res.message_id);
            cJSON_AddStringToObject(response_json, "timestamp", channel_chat_message_res.timestamp);
            vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));
        } else if (channel_chat_message_res.Success == -1) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "DB_INSERT_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message failed to be added to the database.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        } else if (channel_chat_message_res.Success == -2) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "RECIPIENTS_NOT_FOUND");
            cJSON_AddStringToObject(response_json, "message", "Failed to find channel recipients.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        } else {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_SEND_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message failed to be sent.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        }
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);

    } else if (strcmp(chat_type, "personal") == 0) {
        int recipient_id = get_dm_recipient_id(conn, chat_id, sender_id);
        MessageResult_t personal_chat_message_res = handle_personal_chat_message(conn, chat_id, sender_id, recipient_id, message_str,
                                                                     0, 0, 0, 0, 0);

        if (personal_chat_message_res.Success == 1) {
            cJSON_AddBoolToObject(response_json, "error", false);
            cJSON_AddStringToObject(response_json, "status", "success");
            cJSON_AddStringToObject(response_json, "message", "Message sent successfully.");
            cJSON_AddNumberToObject(response_json, "message_id", personal_chat_message_res.message_id);
            cJSON_AddStringToObject(response_json, "timestamp", personal_chat_message_res.timestamp);
            vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));
        } else if (personal_chat_message_res.Success == 2) {
            cJSON_AddBoolToObject(response_json, "error", false);
            cJSON_AddStringToObject(response_json, "status", "success");
            cJSON_AddStringToObject(response_json, "message", "Message added to the database successfully, user is not online.");
            cJSON_AddNumberToObject(response_json, "message_id", personal_chat_message_res.message_id);
            cJSON_AddStringToObject(response_json, "timestamp", personal_chat_message_res.timestamp);
            vendor.server.https.send_https_response(ssl, "201 Created", "application/json", cJSON_Print(response_json));
        } else if (personal_chat_message_res.Success == -1) {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "DB_INSERT_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message failed to be added to the database.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        } else {
            cJSON_AddBoolToObject(response_json, "error", true);
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_SEND_FAILED");
            cJSON_AddStringToObject(response_json, "message", "Message failed to be sent.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        }
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
    } else {
        // TODO: Handle the response where the chat type is unknown
        printf("Error: Unknown chat type\n");
        return;
    }
}

void protected_send_message_rout(SSL *ssl, const char *request) { jwt_middleware(ssl, request, send_message_rout); }
