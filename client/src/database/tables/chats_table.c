#include "../../../inc/header.h"

static void create_table(void) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS chats ("
        "chat_id INTEGER PRIMARY KEY, "    // ID, retrieved from the server
        "chat_type TEXT NOT NULL CHECK (chat_type IN ('personal', 'group', 'channel')), "
        "created_at TIMESTAMP DEFAULT (strftime('%s', 'now'))"
        ");";
    vendor.database.sql.execute_sql(sql);
}

static void add_chat(int chat_id, const char *chat_type) {
    char sql[512];
    snprintf(sql, sizeof(sql),
             "INSERT INTO chats (chat_id, chat_type) VALUES (%d, '%s');",
             chat_id, chat_type);

    vendor.database.sql.execute_sql(sql);
}

static t_chats_struct *get_chat_by_id(int chat_id) {
    char sql[512];
    snprintf(sql, sizeof(sql),
             "SELECT chat_id, chat_type, created_at FROM chats WHERE chat_id = %d;", chat_id);

    char **results = NULL;
    int rows, cols;
    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    if (rc != 0 || rows == 0) {
        printf("No results found for chat_id: %d\n", chat_id);
        return NULL;
    }

    t_chats_struct *chat = (t_chats_struct *)malloc(sizeof(t_chats_struct));
    if (chat == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    chat->chat_id = chat_id;

    switch (results[cols + 1][0]) {
        case 'p':
            chat->chat_type = PERSONAL;
            break;
        case 'g':
            chat->chat_type = GROUP;
            break;
        case 'c':
            chat->chat_type = CHANNEL;
            break;
        default:
            chat->chat_type = PERSONAL;
    }


    time_t timestamp = (time_t)(atoll(results[cols + 2]));
    localtime_r(&timestamp, &chat->created_at);

    return chat;
}

static void free_struct(t_chats_struct *chat) {
    if (chat != NULL) {
        free(chat);
    }
}

static void delete_chat_and_related_data(int chat_id, t_chats_types group_type) {
    char sql[1024];
    snprintf(sql, sizeof(sql),
             "DELETE FROM messages WHERE chat_id = %d;", chat_id);
    vendor.database.sql.execute_sql(sql);

    if (group_type == GROUP) {
        snprintf(sql, sizeof(sql),
                 "DELETE FROM group_info WHERE group_id = %d;", chat_id);
        vendor.database.sql.execute_sql(sql);

        snprintf(sql, sizeof(sql),
                 "DELETE FROM group_chat_members WHERE chat_id = %d;", chat_id);
        vendor.database.sql.execute_sql(sql);
    } else if (group_type == PERSONAL) {
        snprintf(sql, sizeof(sql),
                 "DELETE FROM personal_chats WHERE chat_id = %d;", chat_id);
        vendor.database.sql.execute_sql(sql);
    }

    snprintf(sql, sizeof(sql),
             "DELETE FROM chats WHERE chat_id = %d;", chat_id);
    vendor.database.sql.execute_sql(sql);

    printf("Chat and related data with chat_id %d has been deleted.\n", chat_id);
}

t_chats_table init_chats_table(void) {
    t_chats_table table = {
        .create_table = create_table,
        .add_chat = add_chat,
        .get_chat_by_id = get_chat_by_id,
        .free_struct = free_struct,
        .delete_chat_and_related_data = delete_chat_and_related_data,
    };

    return table;
}
