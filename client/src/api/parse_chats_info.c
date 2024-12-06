#include "../../inc/header.h"

t_chat_info *parse_chat_info_by_id(int chat_id) {
    const char *sql =
        "SELECT chat_id, chat_type, created_at "
        "FROM chats WHERE chat_id = ?;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(vendor.database.db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[ERROR] Failed to prepare SQL statement: %s\n", sqlite3_errmsg(vendor.database.db));
        return NULL;
    }

    sqlite3_bind_int(stmt, 1, chat_id);

    t_chat_info *chat_info = NULL;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        chat_info = malloc(sizeof(t_chat_info));
        if (!chat_info) {
            printf("[ERROR] Memory allocation failed for t_chat_info structure.\n");
            sqlite3_finalize(stmt);
            return NULL;
        }

        chat_info->id = sqlite3_column_int(stmt, 0);

        const char *type_str = (const char *)sqlite3_column_text(stmt, 1);
        chat_info->type = (strcmp(type_str, "personal") == 0) ? PERSONAL :
                          (strcmp(type_str, "group") == 0) ? GROUP : CHANNEL;

        chat_info->timestamp = (time_t)sqlite3_column_int64(stmt, 2);

        if (vendor.debug_mode == 1) {
            printf("[DEBUG] Chat ID: %d\n", chat_info->id);
            printf("[DEBUG] Chat Type: %d\n", chat_info->type);
            printf("[DEBUG] Timestamp: %ld\n", chat_info->timestamp);
        }

        chat_info->path_to_logo = vendor.helpers.strdup("logo_2.jpg");
        chat_info->unreaded_messages = 0;

        if (chat_info->type == PERSONAL) {

            if (vendor.debug_mode == 1) printf("[DEBUG] It's a personalized chat room\n");

            int other_user_id = get_other_user_id(chat_info->id);
            chat_info->name = get_user_name(other_user_id);
            chat_info->path_to_logo = vendor.helpers.strdup(chat_info->name);
            chat_info->last_message = vendor.database.tables.messages_table.get_messages_by_chat_id(chat_info->id, 1, 1, NULL);

        } else if (chat_info->type == GROUP) {
            if (vendor.debug_mode == 1) printf("[DEBUG] It's a group chat\n");

            chat_info->name = get_group_name_by_chat_id(chat_info->id);
            chat_info->last_message = vendor.database.tables.messages_table.get_messages_by_chat_id(chat_info->id, 1, 1, NULL);

        } else {
            chat_info->name = vendor.helpers.strdup("Unknown chat room");
            chat_info->last_message = malloc(sizeof(t_messages_struct));
            chat_info->last_message->message_text = vendor.helpers.strdup("No messages");
        }

        if (vendor.debug_mode == 1) {
            printf("[DEBUG] Chat Info:\n");
            printf("  ID: %d\n", chat_info->id);
            printf("  Type: %d\n", chat_info->type);
            printf("  Timestamp: %ld\n", chat_info->timestamp);
            printf("  Name: %s\n", chat_info->name);
        }
    } else {
        printf("[INFO] No chat found with ID: %d\n", chat_id);
    }

    sqlite3_finalize(stmt);
    return chat_info;
}


t_chat_info **parse_chats_info(void) {
    const char *sql = "SELECT chat_id, chat_type, created_at FROM chats ORDER BY created_at DESC;";
    char **results = NULL;
    int rows, cols;
    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    if (rc != 0) {
        printf("[ERROR] Error during SQL query execution: %s\n", sqlite3_errmsg(vendor.database.db));
        return NULL;
    }

    if (rows == 0) {
        if (vendor.debug_mode >= 1) printf("[INFO] No chats found. The 'chats' table is empty.\n");
        return NULL;
    }

    if (vendor.debug_mode == 1) printf("[DEBUG] Rows found: %d, Columns found: %d\n", rows, cols);

    t_chat_info **chats_info = malloc(sizeof(t_chat_info *) * (rows + 1));
    if (!chats_info) {
        printf("[ERROR] Error of memory allocation for an array of structures.\n");
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        chats_info[i] = malloc(sizeof(t_chat_info));
        if (!chats_info[i]) {
            printf("[ERROR] Error of memory allocation for t_chat_info structure.\n");
            for (int j = 0; j < i; j++) {
                free(chats_info[j]);
            }
            free(chats_info);
            return NULL;
        }

        chats_info[i]->id = atoi(results[(i + 1) * cols]);
        const char *type_str = results[(i + 1) * cols + 1];
        chats_info[i]->type = (strcmp(type_str, "personal") == 0) ? PERSONAL :
                              (strcmp(type_str, "group") == 0) ? GROUP : CHANNEL;
        chats_info[i]->timestamp = (time_t)atoll(results[(i + 1) * cols + 2]);

        if (vendor.debug_mode == 1) {
            printf("[DEBUG] Processing chat #%d\n", i);
            printf("[DEBUG] chat ID: %d\n", chats_info[i]->id);
            printf("[DEBUG] chat type: %d\n", chats_info[i]->type);
            printf("[DEBUG] Timestamp: %ld\n", chats_info[i]->timestamp);
        }

        chats_info[i]->path_to_logo = "logo_2.jpg";
        chats_info[i]->unreaded_messages = 0;

        if (chats_info[i]->type == PERSONAL) {
            if (vendor.debug_mode == 1) printf("[DEBUG] It's a personalized chat room\n");
            int other_user_id = get_other_user_id(chats_info[i]->id);
            chats_info[i]->name = get_user_name(other_user_id);
            chats_info[i]->path_to_logo = vendor.helpers.strdup(chats_info[i]->name);

            chats_info[i]->last_message = vendor.database.tables.messages_table.get_messages_by_chat_id(chats_info[i]->id, 1, 1, NULL);

        } else if (chats_info[i]->type == GROUP) {
           if (vendor.debug_mode == 1)  printf("[DEBUG] It's a group chat\n");
            chats_info[i]->name = get_group_name_by_chat_id(chats_info[i]->id);
            chats_info[i]->last_message = vendor.database.tables.messages_table.get_messages_by_chat_id(chats_info[i]->id, 1, 1, NULL);
        } else {
            chats_info[i]->name = vendor.helpers.strdup("Unknown chat room");
            chats_info[i]->last_message->message_text = vendor.helpers.strdup("No messages");
        }
        if (vendor.debug_mode == 1)  {
            printf("[DEBUG] Chat #%d\n", i);
            printf("  ID: %d\n", chats_info[i]->id);
            printf("  Type: %d\n", chats_info[i]->type);
            printf("  Timestamp: %ld\n", chats_info[i]->timestamp);
            printf("  Name: %s\n", chats_info[i]->name);
        }
    }

    chats_info[rows] = NULL;

    qsort(chats_info, rows, sizeof(t_chat_info *), compare_chats);

    sqlite3_free_table(results);

    return chats_info;
}

void free_chats_info(t_chat_info **chats_info) {
    if (chats_info != NULL) {
        for (int i = 0; chats_info[i] != NULL; i++) {
            free(chats_info[i]);
        }
        free(chats_info);
    }
}

char *format_timestamp(struct tm timestamp) {
    time_t now = time(NULL);
    struct tm *now_tm = localtime(&now);

    time_t now_seconds = mktime(now_tm);
    time_t timestamp_seconds = mktime(&timestamp);

    double difference = difftime(now_seconds, timestamp_seconds);

    char *buffer = malloc(50);
    if (!buffer) return NULL;

    if (difference < 24 * 60 * 60) {
        strftime(buffer, 50, "%H:%M", &timestamp);
    } else if (difference < 7 * 24 * 60 * 60) {
        strftime(buffer, 50, "%a", &timestamp);
    } else {
        strftime(buffer, 50, "%d/%m/%Y", &timestamp);
    }

    return buffer;
}

int compare_chats(const void *a, const void *b) {
    t_chat_info *chat_a = *(t_chat_info **)a;
    t_chat_info *chat_b = *(t_chat_info **)b;

    time_t time_a = chat_a->last_message ? mktime(&chat_a->last_message->timestamp) : chat_a->timestamp;
    time_t time_b = chat_b->last_message ? mktime(&chat_b->last_message->timestamp) : chat_b->timestamp;

    if (time_a > time_b) return -1;
    if (time_a < time_b) return 1;
    return 0;
}
