#include "../../../inc/handlers/post_handlers/group_add.h"

// Helper function prototypes
static PGresult *get_group_member(PGconn *conn, int chat_id, int user_id);
static bool add_user_to_group(PGconn *conn, int chat_id, int user_id);
static bool personal_chat_exists(PGconn *conn, int user1_id, int user2_id);

void group_add_rout(SSL *ssl, const char *request) {
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

    // Extract and validate sender_id, recipient_id, and chat_id
    char *chat_id_str = extract_chat_id(json);
    char *user_id_str = extract_user_id(json);
    char *sender_id_str = get_sender_id_from_token(request);

    if (!chat_id_str || !is_valid_chat_id(chat_id_str) || !user_id_str || !sender_id_str) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_PARAMETERS");
        cJSON_AddStringToObject(response_json, "message", "Invalid or missing parameters");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        free(chat_id_str);
        free(user_id_str);
        free(sender_id_str);
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

    if (!is_valid_user_id(user_id_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_USER_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid user ID");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    int chat_id = atoi(chat_id_str);
    int user_id = atoi(user_id_str);
    int sender_id = atoi(sender_id_str);
    printf("[DEBUG] chat_id: %d, user_id: %d, sender_id: %d\n", chat_id, user_id, sender_id);

    free(chat_id_str);
    free(user_id_str);
    free(sender_id_str);

    if (sender_id == user_id) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_USER_IDS");
        cJSON_AddStringToObject(response_json, "message", "Cannot create a personal chat with yourself");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    // Database connection
    PGconn *conn = vendor.database.pool.acquire_connection();
    if (PQstatus(conn) != CONNECTION_OK) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "DB_CONNECTION_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Database connection failed");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    // Check if personal chat exists
    if (!personal_chat_exists(conn, sender_id, user_id)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "NOT_FRIENDS");
        cJSON_AddStringToObject(response_json, "message", "You can only add friends to the group");
        vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }

    // Check if the user is already in the group
    PGresult *member_check = get_group_member(conn, chat_id, user_id);
    if (member_check && PQntuples(member_check) > 0) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "USER_ALREADY_IN_GROUP");
        cJSON_AddStringToObject(response_json, "message", "User is already a member of the group");
        vendor.server.https.send_https_response(ssl, "409 Conflict", "application/json", cJSON_Print(response_json));
        PQclear(member_check);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }
    PQclear(member_check);

    // Add user to the group
    if (!add_user_to_group(conn, chat_id, user_id)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "ADD_MEMBER_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Failed to add user to the group");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }

    // Send WebSocket notification
    if (is_user_online(user_id)) {
        cJSON *ws_message = cJSON_CreateObject();
        cJSON_AddNumberToObject(ws_message, "chat_id", chat_id);
        cJSON_AddNumberToObject(ws_message, "user_id", user_id);
        cJSON_AddStringToObject(ws_message, "message", "You have been added to a group");
        _send_message_to_client(user_id, ws_message);
        cJSON_Delete(ws_message);
    }

    // Respond with success
    cJSON_AddBoolToObject(response_json, "error", false);
    cJSON_AddStringToObject(response_json, "message", "User successfully added to the group");
    vendor.server.https.send_https_response(ssl, "201 Created", "application/json", cJSON_Print(response_json));

    // Cleanup
    cJSON_Delete(json);
    cJSON_Delete(response_json);
    vendor.database.pool.release_connection(conn);

    return;
}

void protected_group_add_rout(SSL *ssl, const char *request) {
    jwt_middleware(ssl, request, group_add_rout);
}
// Database helper functions

static PGresult *get_group_member(PGconn *conn, int chat_id, int user_id) {
    const char *query = "SELECT * FROM group_chat_members WHERE chat_id = $1 AND user_id = $2";

    char chat_id_str[12], user_id_str[12];
    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);
    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);

    const char *params[2] = {chat_id_str, user_id_str};
    return PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);
}

static bool add_user_to_group(PGconn *conn, int chat_id, int user_id) {
    const char *query = "INSERT INTO group_chat_members (chat_id, user_id, role) VALUES ($1, $2, 'member')";

    char chat_id_str[12], user_id_str[12];
    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);
    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);

    const char *params[2] = {chat_id_str, user_id_str};
    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);
    bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);
    PQclear(res);
    return success;
}

static bool personal_chat_exists(PGconn *conn, int user1_id, int user2_id) {
    PGresult *res = get_personal_chat(conn, user1_id, user2_id);
    bool exists = (res && PQntuples(res) > 0);
    PQclear(res);
    return exists;
}
