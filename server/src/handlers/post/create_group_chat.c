#include "../../../inc/handlers/post_handlers/create_group_chat.h"
#include <string.h>

int create_group_chat_no_media(PGconn *conn, const char *group_name) {
    const char *query = "WITH new_chat AS (INSERT INTO chats (chat_type) VALUES ('group') RETURNING chat_id) "
                        "INSERT INTO group_chats (chat_id, group_name) "
                        "SELECT chat_id, $1 FROM new_chat "
                        "RETURNING chat_id";

    const char *params[1] = {group_name};

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Create group chat failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    int chat_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return chat_id;
}

// Extract group name
char *extract_group_name(cJSON *json) {
    cJSON *group_name_item = cJSON_GetObjectItem(json, "group_name");
    if (group_name_item && cJSON_IsString(group_name_item)) {
        return strdup(group_name_item->valuestring); // Allocate memory for the string
    }
    return NULL;
}

// Extract group picture
int extract_group_picture(cJSON *json) {
    cJSON *group_picture_item = cJSON_GetObjectItem(json, "group_picture");
    if (group_picture_item && cJSON_IsNumber(group_picture_item)) {
        return group_picture_item->valueint;
    }
    return -1; // Return -1 to indicate an invalid or missing value
}

// Extract background
int extract_background(cJSON *json) {
    cJSON *background_item = cJSON_GetObjectItem(json, "background");
    if (background_item && cJSON_IsNumber(background_item)) {
        return background_item->valueint;
    }
    return -1; // Return -1 to indicate an invalid or missing value
}

void create_group_chat_rout(SSL *ssl, const char *request) {
    char *body = strstr(request, "\r\n\r\n");
    cJSON *response_json = cJSON_CreateObject();

    // Check if the request body exists
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

    // Check if JSON is valid
    if (!json) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_JSON");
        cJSON_AddStringToObject(response_json, "message", "Invalid JSON format");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    // Extract data
    char *group_name = extract_group_name(json);
    int group_picture = extract_group_picture(json);
    int background = extract_background(json);
    group_picture = background;

    if (!group_name || group_picture < 0 || background < 0) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_INPUT");
        cJSON_AddStringToObject(response_json, "message", "Invalid group_name, group_picture, or background");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        if (group_name) free(group_name);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }


    // Extract user ID from JWT
    char *user_id_str = get_sender_id_from_token(request);
    if (!is_valid_user_id(user_id_str)) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_USER_ID");
        cJSON_AddStringToObject(response_json, "message", "Invalid user ID");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        free(user_id_str);
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        return;
    }

    int user_id = atoi(user_id_str);
    free(user_id_str);

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

    // Insert group chat into the database
    printf("[INFO] Creating group chat with name: %s, picture: %d, background: %d\n", group_name, group_picture, background);
    int chat_id = create_group_chat_no_media(conn, group_name);

    if (!add_chat_member(conn, chat_id, user_id, "owner")) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "ADD_OWNER_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Failed to add owner to the group");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(json);
        cJSON_Delete(response_json);
        vendor.database.pool.release_connection(conn);
        return;
    }

    vendor.database.pool.release_connection(conn);

    if (chat_id == -1) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "DB_INSERT_FAILED");
        cJSON_AddStringToObject(response_json, "message", "Failed to create group chat");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
    } else {
        cJSON_AddBoolToObject(response_json, "error", false);
        cJSON_AddStringToObject(response_json, "code", "OK");
        cJSON_AddStringToObject(response_json, "message", "Group chat created successfully");
        cJSON_AddNumberToObject(response_json, "chat_id", chat_id);
        cJSON_AddNumberToObject(response_json, "creator_id", user_id);
        vendor.server.https.send_https_response(ssl, "201 Created", "application/json", cJSON_Print(response_json));
    }

    cJSON_Delete(json);
    cJSON_Delete(response_json);
}

void protected_create_group_chat_rout(SSL *ssl, const char *request) { jwt_middleware(ssl, request, create_group_chat_rout); }
