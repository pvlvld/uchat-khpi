#include "../../../../inc/handlers/get_handlers/get_all_updates_after_offline.h"

PGresult *get_chat_by_id(PGconn *conn, int chat_id) {
    const char *query = "SELECT created_at FROM chats WHERE chat_id = $1";

    char chat_id_str[12];
    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);
    const char *params[1] = { chat_id_str };

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query to get chat by id failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    return res; // Caller is responsible for freeing with PQclear.
}

PGresult *is_chat_created(PGconn *conn, int chat_id, time_t timestamp) {
    const char *query =
        "SELECT chat_id, created_at "
        "FROM chats "
        "WHERE chat_id = $1 AND created_at > to_timestamp($2)";

    char chat_id_str[12];
    char timestamp_str[20];

    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);
    snprintf(timestamp_str, sizeof(timestamp_str), "%ld", timestamp);

    const char *params[2] = {chat_id_str, timestamp_str};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error checking chat creation time: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    // If no rows are returned, it means the chat was not created after the timestamp.
    if (PQntuples(res) == 0) {
        PQclear(res);
        return NULL;
    }

    return res; // Caller is responsible for freeing the result with PQclear.
}


void process_new_users_in_chats(PGconn *conn, PGresult *personal_chat_list, time_t timestamp, int user_id, cJSON *new_users) {
    if (!personal_chat_list || !conn || !new_users) {
        fprintf(stderr, "Invalid arguments to process_new_users_in_chats\n");
        return;
    }

    int rows = PQntuples(personal_chat_list);

    for (int i = 0; i < rows; i++) {
        // Fetch the chat_id from the personal_chat_list
        const char *chat_id_str = PQgetvalue(personal_chat_list, i, 0); // Assuming "chat_id" is the first column
        int chat_id = atoi(chat_id_str);

        // Check if the chat was created after the given timestamp
        PGresult *chat_res = is_chat_created(conn, chat_id, timestamp);
        if (!chat_res) {
            continue; // Skip this chat if it wasn't created after the timestamp
        }
        const char *timestamp = PQgetvalue(chat_res, 0, 1);

        // Determine the other user in the chat
        const char *user1_id_str = PQgetvalue(personal_chat_list, i, 1); // Assuming "user1_id" is the second column
        const char *user2_id_str = PQgetvalue(personal_chat_list, i, 2); // Assuming "user2_id" is the third column
        int user1_id = atoi(user1_id_str);
        int user2_id = atoi(user2_id_str);
        int other_user_id = (user1_id == user_id) ? user2_id : user1_id;

        // Fetch the details of the other user
        PGresult *user_res = get_user_by_id(conn, other_user_id);
        if (!user_res || PQresultStatus(user_res) != PGRES_TUPLES_OK || PQntuples(user_res) < 1) {
            fprintf(stderr, "Failed to fetch user details for user_id: %d\n", other_user_id);
            if (user_res) PQclear(user_res);
            continue;
        }

        // Extract user details
        const char *username = PQgetvalue(user_res, 0, 0);
        const char *user_login = PQgetvalue(user_res, 0, 1);
        const char *profile_picture = PQgetvalue(user_res, 0, 2);
        const char *public_key = PQgetvalue(user_res, 0, 8);

        // Populate JSON object for the user
        cJSON *user_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(user_json, "user_id", other_user_id);
        cJSON_AddStringToObject(user_json, "username", username ? username : "");
        cJSON_AddStringToObject(user_json, "user_login", user_login ? user_login : "");
        cJSON_AddStringToObject(user_json, "profile_picture", profile_picture ? profile_picture : "");
        cJSON_AddStringToObject(user_json, "public_key", public_key ? public_key : "");
        cJSON_AddStringToObject(user_json, "timestamp", timestamp ? timestamp : "");
        // Add the JSON object to the new_users array
        cJSON_AddItemToArray(new_users, user_json);

        // Clean up the chat result, as we only needed to check its existence
        PQclear(chat_res);
        PQclear(user_res); // Clean up user result
    }
}


void get_updates_personal_chats(PGconn *conn, PGresult *chat_list, cJSON *personal_chats, int user_id, time_t timestamp) {
    for (int i = 0; i < PQntuples(chat_list); i++) {
        int chat_id = atoi(PQgetvalue(chat_list, i, 0));

        cJSON *chat_updates = cJSON_CreateObject();
        cJSON_AddNumberToObject(chat_updates, "chat_id", chat_id);

        // Updated users
        cJSON *updated_user = cJSON_CreateObject();
        PGresult *user_update = is_user_updated(conn, user_id, timestamp);

        if (user_update && PQresultStatus(user_update) == PGRES_TUPLES_OK && PQntuples(user_update) > 0) {
            // Extract the single user's details (row 0)
            int updated_user_id = atoi(PQgetvalue(user_update, 0, 0));
            char *updated_username = PQgetvalue(user_update, 0, 1);
            char *updated_user_login = PQgetvalue(user_update, 0, 2);

            // Add the details to the JSON object
            cJSON_AddNumberToObject(updated_user, "user_id", updated_user_id);
            cJSON_AddStringToObject(updated_user, "username", updated_username);
            cJSON_AddStringToObject(updated_user, "user_login", updated_user_login);
        }

        // Cleanup
        if (user_update) {
            PQclear(user_update);
        }

        cJSON_AddItemToObject(chat_updates, "updated_users", updated_user);

        // New messages
        cJSON *new_messages = cJSON_CreateArray();
        int limit = 200; // Fetch 200 messages per batch
        int offset = 0;  // Start from the beginning
        PGresult *messages = NULL;
        int num_rows = 0;

        // Fetch messages in chunks
        while (1) {
            // Fetch the messages with the current limit and offset
            messages = get_new_messages(conn, chat_id, limit, offset, timestamp);

            if (!messages || PQresultStatus(messages) != PGRES_TUPLES_OK) {
                fprintf(stderr, "Failed to fetch messages: %s\n", PQerrorMessage(conn));
                if (messages) PQclear(messages); // Cleanup if query failed
                break;
            }

            // Get the number of rows in the result
            num_rows = PQntuples(messages);

            // Process each message in the current batch
            for (int k = 0; k < num_rows; k++) {
                // Create a JSON object for each message
                cJSON *message = cJSON_CreateObject();

                // Extract relevant fields from the result
                int message_id = atoi(PQgetvalue(messages, k, 0));        // message_id (Column 0)
                int sender_id = atoi(PQgetvalue(messages, k, 2));         // sender_id (Column 3)
                char *content = PQgetvalue(messages, k, 3);               // message_text (Column 4)
                char *edited_at = PQgetvalue(messages, k, 5);             // edited_at (Column 6)
                char *deleted_at = PQgetvalue(messages, k, 6);            // deleted_at (Column 7)
                char *timestamp = PQgetvalue(messages, k, 12);            // timestamp (Column 12)

                printf("[DEBUG] message_id %d\n", message_id);

                if (edited_at) printf("[DEBUG] edited_at %s\n", edited_at);
                if (deleted_at) printf("[DEBUG] deleted_at %s\n", deleted_at);
                // Add the extracted fields to the JSON object
                cJSON_AddNumberToObject(message, "message_id", message_id);
                cJSON_AddNumberToObject(message, "sender_id", sender_id);
                cJSON_AddStringToObject(message, "content", content ? content : "");
                cJSON_AddStringToObject(message, "timestamp", timestamp ? timestamp : "");
                if (edited_at) cJSON_AddStringToObject(message, "edited_at", strlen(edited_at) > 0 ? edited_at : "");
                cJSON_AddBoolToObject(message, "deleted", deleted_at && strlen(deleted_at) > 0);

                // Add the JSON object to the array
                cJSON_AddItemToArray(new_messages, message);
            }

            // Free the result object for the current batch
            PQclear(messages);

            // Check if we have fetched all the messages
            if (num_rows < limit) {
                // If fewer rows are returned than the limit, no more messages to fetch
                break;
            }

            // Move to the next batch
            offset += limit;
        }

        cJSON_AddItemToObject(chat_updates, "new_messages", new_messages);

        // Add to `personal_chats`
        cJSON_AddItemToArray(personal_chats, chat_updates);
    }
}


void get_updates_group_chats(PGconn *conn, PGresult *group_chat_list, cJSON *group_chats, int user_id, time_t timestamp) {
    for (int i = 0; i < PQntuples(group_chat_list); i++) {
        int chat_id = atoi(PQgetvalue(group_chat_list, i, 0));

        cJSON *chat_updates = cJSON_CreateObject();
        cJSON_AddNumberToObject(chat_updates, "chat_id", chat_id);

        // Updated group info
        PGresult *group_info = is_group_updated(conn, chat_id, timestamp);
        if (group_info && PQresultStatus(group_info) == PGRES_TUPLES_OK) {
            cJSON_AddStringToObject(chat_updates, "group_name", PQgetvalue(group_info, 0, 1));
        }
        PQclear(group_info);

        // Updated users
        cJSON *updated_users = cJSON_CreateArray();
        PGresult *group_members = get_group_members(conn, chat_id);
        for (int j = 0; j < PQntuples(group_members); j++) {
            int member_id = atoi(PQgetvalue(group_members, j, 0));
            if (member_id == user_id) continue;
            PGresult *user_update = is_user_updated(conn, member_id, timestamp);

            if (user_update && PQresultStatus(user_update) == PGRES_TUPLES_OK && PQntuples(user_update) > 0) {
                // Extract user details
                char *updated_username = PQgetvalue(user_update, 0, 1);
                char *updated_user_login = PQgetvalue(user_update, 0, 2);

                // Create JSON object for the updated user
                cJSON *user_json = cJSON_CreateObject();
                cJSON_AddNumberToObject(user_json, "user_id", member_id);
                cJSON_AddStringToObject(user_json, "username", updated_username);
                cJSON_AddStringToObject(user_json, "user_login", updated_user_login);

                // Add the JSON object to the array
                cJSON_AddItemToArray(updated_users, user_json);
            }

            PQclear(user_update);
        }

        PQclear(group_members);
        cJSON_AddItemToObject(chat_updates, "updated_users", updated_users);

        // New messages
        cJSON *new_messages = cJSON_CreateArray();
        int limit = 200; // Fetch 200 messages per batch
        int offset = 0;  // Start from the beginning
        PGresult *messages = NULL;
        int num_rows = 0;

        // Fetch messages in chunks
        while (1) {
            // Fetch the messages with the current limit and offset
            messages = get_new_messages(conn, chat_id, limit, offset, timestamp);

            if (!messages || PQresultStatus(messages) != PGRES_TUPLES_OK) {
                fprintf(stderr, "Failed to fetch messages: %s\n", PQerrorMessage(conn));
                if (messages) PQclear(messages); // Cleanup if query failed
                break;
            }

            // Get the number of rows in the result
            num_rows = PQntuples(messages);

            // Process each message in the current batch
            for (int k = 0; k < num_rows; k++) {
                // Create a JSON object for each message
                cJSON *message = cJSON_CreateObject();

                // Extract relevant fields from the result
                int message_id = atoi(PQgetvalue(messages, k, 0));        // message_id (Column 0)
                int sender_id = atoi(PQgetvalue(messages, k, 2));         // sender_id (Column 3)
                char *content = PQgetvalue(messages, k, 3);               // message_text (Column 4)
                char *edited_at = PQgetvalue(messages, k, 5);             // edited_at (Column 6)
                char *deleted_at = PQgetvalue(messages, k, 6);            // deleted_at (Column 7)
                char *timestamp = PQgetvalue(messages, k, 11);            // timestamp (Column 12)

                // Add the extracted fields to the JSON object
                cJSON_AddNumberToObject(message, "message_id", message_id);
                cJSON_AddNumberToObject(message, "sender_id", sender_id);
                cJSON_AddStringToObject(message, "content", content ? content : "");
                cJSON_AddStringToObject(message, "timestamp", timestamp ? timestamp : "");
                cJSON_AddStringToObject(message, "edited_at", edited_at && strlen(edited_at) > 0 ? edited_at : "false");
                cJSON_AddStringToObject(message, "deleted", deleted_at && strlen(deleted_at) > 0 ? "true" : "false");

                // Add the JSON object to the array
                cJSON_AddItemToArray(new_messages, message);
            }

            // Free the result object for the current batch
            PQclear(messages);

            // Check if we have fetched all the messages
            if (num_rows < limit) {
                // If fewer rows are returned than the limit, no more messages to fetch
                break;
            }

            // Move to the next batch
            offset += limit;
        }

        cJSON_AddItemToObject(chat_updates, "new_messages", new_messages);

        // Add to `group_chats`
        cJSON_AddItemToArray(group_chats, chat_updates);
    }
}



bool fetch_updates_from_database(PGconn *conn, int user_id, time_t timestamp, cJSON *updates) {
    // Initialize `personal_chats` and `group_chats` objects
    cJSON *new_users = cJSON_CreateArray();
    cJSON *personal_chats = cJSON_CreateArray();
    cJSON *group_chats = cJSON_CreateArray();

    // Fetch and update personal chats
    PGresult *personal_chat_list = get_user_chats_personal(conn, user_id);

    if (PQresultStatus(personal_chat_list) == PGRES_TUPLES_OK) {
        process_new_users_in_chats(conn, personal_chat_list, timestamp, user_id, new_users);
        get_updates_personal_chats(conn, personal_chat_list, personal_chats, user_id, timestamp);
    }
    PQclear(personal_chat_list);

    // Fetch and update group chats
    PGresult *group_chat_list = get_user_chats_group(conn, user_id);
    if (PQresultStatus(group_chat_list) == PGRES_TUPLES_OK) {
        get_updates_group_chats(conn, group_chat_list, group_chats, user_id, timestamp);
    }
    PQclear(group_chat_list);

    // Add personal and group chats to the updates JSON
    cJSON_AddItemToObject(updates, "new_users", new_users);
    cJSON_AddItemToObject(updates, "personal_chats", personal_chats);
    cJSON_AddItemToObject(updates, "group_chats", group_chats);

    return true;
}
