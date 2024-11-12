#include "../../../inc/header.h"
#include "../../../inc/utils.h"

void send_message_rout(SSL *ssl, const char *request) {
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

    char *chat_id_str = extract_chat_id(json);
    char *message_str = extract_message(json);
    char *sender_login_str = get_sender_from_token(request);

    if (!is_valid_chat_id(chat_id)) {
        cJSON_AddStringToObject(response_json, "message", "Invalid recipient login.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));

        cJSON_Delete(response_json);
        free(message_str);
        free(chat_id_str);
        free(sender_login_str);
        return;
    }

    if (!is_valid_login(sender_login_str)) {
        cJSON_AddStringToObject(response_json, "message", "Invalid sender login.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));

        cJSON_Delete(response_json);
        free(message_str);
        free(chat_id_str);
        free(sender_login_str);
        return;
    }

    if (!is_valid_message(message_str)) { // TODO: check validity for every message type we support
        cJSON_AddStringToObject(
            response_json, "message",
            "Invalid message."); // 1 - 5000 characters, only letters, numbers and special characters for now
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));

        cJSON_Delete(response_json);
        free(message_str);
        free(chat_id_str);
        free(sender_login_str);
        return;
    }

    int chat_id = atoi(chat_id_str);

    // Connect to the database
    PGconn *conn = vendor.database.pool.acquire_connection();
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        cJSON_AddStringToObject(response_json, "message", "Database connection failed");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json",
                                                cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    PGresult *res = get_user_by_login(conn, sender_login_str);
    if (res == NULL) {
        PQclear(res);
        cJSON_AddStringToObject(response_json, "message", "The login, received from the token, does not exist.");
        vendor.server.https.send_https_response(ssl, "409 Conflict", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
        return;
    }

    int sender_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    // Check if user is part of the chat
    if (!is_user_in_chat(conn, chat_id, sender_id)) {
        cJSON_AddStringToObject(response_json, "message",
                                "The sender has no access to this chat. Or chat id is invalid.");
        vendor.server.https.send_https_response(ssl, "409 Conflict", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        PQfinish(conn);
    }

    const char *chat_type = get_chat_type(conn, chat_id);
    if (strcmp(chat_type, "group") == 0) {
        // For group chats, check user permissions
        const char *user_role = get_user_role_in_group(conn, chat_id, sender_id);
        if (!user_role || strcmp(user_role, "banned") == 0 || strcmp(user_role, "restricted") == 0) {
            // TODO: Handle the response where the user is banned or restricted
            printf("Error: User does not have permission to send messages in this group\n");
            return;
        }

        // TODO: Proceed with storing and sending group message logic here

    } else if (strcmp(chat_type, "channel") == 0) {
        // For channels, ensure the user is the owner
        const char *user_role = get_user_role_in_group(conn, chat_id, sender_id);
        if (!user_role || strcmp(user_role, "owner") != 0) {
            // TODO: Handle the response where the user isn't the owner
            printf("Error: Only the owner can send messages in this channel\n");
            return;
        }

        // TODO: Proceed with storing and sending channel message logic here

    } else if (strcmp(chat_type, "personal") == 0) {
        // TODO: For personal chats, proceed without further permission checks

    } else {
        printf("Error: Unknown chat type\n");
        return;
    }
}

void protected_send_message_rout(SSL *ssl, const char *request) { jwt_middleware(ssl, request, send_message_rout); }
