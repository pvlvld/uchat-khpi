#include "../../../inc/handlers/post_handlers/group_add.h"

cJSON *get_full_group_info(PGconn *conn, int chat_id, int sender_id);
void send_full_group_info_to_user(PGconn *conn, int chat_id, int user_id, int sender_id);
void notify_all_group_members(PGconn *conn, int chat_id, int new_member_id, const char *new_member_username);
static bool personal_chat_exists(PGconn *conn, int user1_id, int user2_id);
static PGresult *get_group_member(PGconn *conn, int chat_id, int user_id);

void add_user_to_group_rout(SSL *ssl, const char *request) {
    char *body = strstr(request, "\r\n\r\n");
    cJSON *response_json = cJSON_CreateObject();

    // Step 1: Check for request body
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

    // Step 2: Parse and validate JSON
    if (!json) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_JSON");
        cJSON_AddStringToObject(response_json, "message", "Invalid JSON format");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    char *chat_id_str = extract_chat_id(json);
    char *user_username_str = extract_recipient_username(json);
    char *sender_id_str = get_sender_id_from_token(request);

    if (!chat_id_str || !user_username_str || !sender_id_str) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_PARAMETERS");
        cJSON_AddStringToObject(response_json, "message", "Invalid or missing parameters");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        free(chat_id_str);
        free(user_username_str);
        free(sender_id_str);
        return;
    }

    int chat_id = atoi(chat_id_str);
    int sender_id = atoi(sender_id_str);

    // Step 3: Ensure sender and recipient are not the same
    PGconn *conn = vendor.database.pool.acquire_connection();
    if (PQstatus(conn) != CONNECTION_OK) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "DB_CONNECTION_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Database connection failed");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json",
                                                cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        free(chat_id_str);
        free(user_username_str);
        free(sender_id_str);
        return;
    }

    PGresult *user_result = get_user_by_username(conn, user_username_str);
    if (!user_result || PQntuples(user_result) == 0) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "USER_NOT_FOUND");
        cJSON_AddStringToObject(response_json, "message", "Recipient user not found");
        vendor.server.https.send_https_response(ssl, "404 Not Found", "application/json", cJSON_Print(response_json));
        PQclear(user_result);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        free(chat_id_str);
        free(user_username_str);
        free(sender_id_str);
        vendor.database.pool.release_connection(conn);
        return;
    }

    int user_id = atoi(PQgetvalue(user_result, 0, 0));
    PQclear(user_result);

    if (sender_id == user_id) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_REQUEST");
        cJSON_AddStringToObject(response_json, "message", "Cannot add yourself to the group");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        free(chat_id_str);
        free(user_username_str);
        free(sender_id_str);
        vendor.database.pool.release_connection(conn);
        return;
    }

    // Step 4: Ensure a personal chat exists
    if (!personal_chat_exists(conn, sender_id, user_id)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "NOT_FRIENDS");
        cJSON_AddStringToObject(response_json, "message", "You can only add friends to the group");
        vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        free(chat_id_str);
        free(user_username_str);
        free(sender_id_str);
        vendor.database.pool.release_connection(conn);
        return;
    }

    // Step 5: Ensure user is not already in the group
    PGresult *group_member_result = get_group_member(conn, chat_id, user_id);
    if (group_member_result && PQntuples(group_member_result) > 0) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "USER_ALREADY_IN_GROUP");
        cJSON_AddStringToObject(response_json, "message", "User is already a member of the group");
        vendor.server.https.send_https_response(ssl, "409 Conflict", "application/json", cJSON_Print(response_json));
        PQclear(group_member_result);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        free(chat_id_str);
        free(user_username_str);
        free(sender_id_str);
        vendor.database.pool.release_connection(conn);
        return;
    }
    PQclear(group_member_result);

    // Step 6: Add user to the group
    if (!add_chat_member(conn, chat_id, user_id, "member")) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "ADD_MEMBER_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Failed to add user to the group");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json",
                                                cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        free(chat_id_str);
        free(user_username_str);
        free(sender_id_str);
        vendor.database.pool.release_connection(conn);
        return;
    }

    // Step 7: Send WebSocket notifications
    send_full_group_info_to_user(conn, chat_id, user_id, sender_id);                // Notify added user about group and members
    notify_all_group_members(conn, chat_id, user_id, user_username_str); // Notify group members about the new user

    // Step 8: Respond with success
    cJSON_AddBoolToObject(response_json, "error", false);
    cJSON_AddStringToObject(response_json, "message", "User successfully added to the group");
    vendor.server.https.send_https_response(ssl, "201 Created", "application/json", cJSON_Print(response_json));

    // Cleanup
    cJSON_Delete(json);
    cJSON_Delete(response_json);
    free(chat_id_str);
    free(user_username_str);
    free(sender_id_str);
    vendor.database.pool.release_connection(conn);
}

void protected_add_user_to_group_rout(SSL *ssl, const char *request) {
    jwt_middleware(ssl, request, add_user_to_group_rout);
}

void send_full_group_info_to_user(PGconn *conn, int chat_id, int user_id, int sender_id) {
    cJSON *group_info = get_full_group_info(conn, chat_id, sender_id);
    _send_message_to_client(user_id, group_info);
    cJSON_Delete(group_info);
}

void notify_all_group_members(PGconn *conn, int chat_id, int new_member_id, const char *new_member_username) {
    PGresult *group_members = get_group_members(conn, chat_id);
    for (int i = 0; i < PQntuples(group_members); i++) {
        int member_id = atoi(PQgetvalue(group_members, i, 0));
        printf("Member ID: %d\n", member_id);
        if (member_id != new_member_id) {
            cJSON *notification = cJSON_CreateObject();
            cJSON_AddNumberToObject(notification, "sender_id", new_member_id);
            cJSON_AddStringToObject(notification, "message", "group_member_added");
            cJSON_AddNumberToObject(notification, "chat_id", chat_id);
            cJSON_AddNumberToObject(notification, "user_id", new_member_id);
            cJSON_AddStringToObject(notification, "username", new_member_username);
            _send_message_to_client(member_id, notification);
            cJSON_Delete(notification);
        }
    }
    PQclear(group_members);
}

cJSON *get_full_group_info(PGconn *conn, int chat_id, int sender_id) {
    // Initialize the group_info JSON object
    cJSON *group_info = cJSON_CreateObject();

    // Query to get group details
    const char *group_query = "SELECT group_name, COALESCE(group_picture, 0) AS group_picture "
                              "FROM group_chats WHERE chat_id = $1";

    char chat_id_str[12];
    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);

    const char *group_params[1] = {chat_id_str};
    PGresult *group_res = PQexecParams(conn, group_query, 1, NULL, group_params, NULL, NULL, 0);

    if (PQresultStatus(group_res) != PGRES_TUPLES_OK || PQntuples(group_res) == 0) {
        fprintf(stderr, "Failed to get group details: %s\n", PQerrorMessage(conn));
        PQclear(group_res);
        cJSON_Delete(group_info);
        return NULL;
    }

    // Add group details to group_info JSON
    cJSON_AddStringToObject(group_info, "message", "group_info");
    cJSON_AddNumberToObject(group_info, "sender_id", sender_id);
    cJSON_AddNumberToObject(group_info, "chat_id", chat_id);
    cJSON_AddStringToObject(group_info, "group_name", PQgetvalue(group_res, 0, 0));
    cJSON_AddNumberToObject(group_info, "group_picture", atoi(PQgetvalue(group_res, 0, 1)));

    PQclear(group_res);

    // Query to get group members
    const char *members_query =
        "SELECT users.user_id, users.username, COALESCE(users.profile_picture, 0) AS profile_picture "
        "FROM group_chat_members "
        "JOIN users ON group_chat_members.user_id = users.user_id "
        "WHERE group_chat_members.chat_id = $1";

    PGresult *members_res = PQexecParams(conn, members_query, 1, NULL, group_params, NULL, NULL, 0);

    if (PQresultStatus(members_res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Failed to get group members: %s\n", PQerrorMessage(conn));
        PQclear(members_res);
        cJSON_Delete(group_info);
        return NULL;
    }

    // Create the group_member array
    cJSON *group_members = cJSON_CreateArray();

    for (int i = 0; i < PQntuples(members_res); i++) {
        cJSON *member = cJSON_CreateObject();

        // Add member details to the member JSON object
        cJSON_AddNumberToObject(member, "member_id", atoi(PQgetvalue(members_res, i, 0)));
        cJSON_AddStringToObject(member, "member_username", PQgetvalue(members_res, i, 1));
        cJSON_AddNumberToObject(member, "member_profile_picture", atoi(PQgetvalue(members_res, i, 2)));

        // Add the member object to the group_members array
        cJSON_AddItemToArray(group_members, member);
    }

    PQclear(members_res);

    // Add group_members array to the group_info JSON object
    cJSON_AddItemToObject(group_info, "group_members", group_members);

    return group_info;
}

static bool personal_chat_exists(PGconn *conn, int user1_id, int user2_id) {
    PGresult *res = get_personal_chat(conn, user1_id, user2_id);
    bool exists = (res && PQntuples(res) > 0);
    PQclear(res);
    return exists;
}

static PGresult *get_group_member(PGconn *conn, int chat_id, int user_id) {
    const char *query = "SELECT * FROM group_chat_members WHERE chat_id = $1 AND user_id = $2";

    char chat_id_str[12], user_id_str[12];
    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);
    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);

    const char *params[2] = {chat_id_str, user_id_str};
    return PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);
}
