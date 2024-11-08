#include "../../../inc/header.h"

static void create_table(void) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS messages ("
        "message_id INTEGER PRIMARY KEY, "
        "chat_id INTEGER NOT NULL, "
        "sender_id INTEGER NOT NULL, "
        "message_text TEXT NOT NULL, "
        "timestamp TIMESTAMP DEFAULT (strftime('%s', 'now')), "
        "edited_at TIMESTAMP DEFAULT NULL, "
        "read_at TIMESTAMP DEFAULT NULL, "
        "FOREIGN KEY (chat_id) REFERENCES chats(chat_id) ON DELETE CASCADE, "
        "FOREIGN KEY (sender_id) REFERENCES users(user_id) ON DELETE CASCADE"
        ");";

    vendor.database.sql.execute_sql(sql);
}

static void add_message(int message_id, int chat_id, int sender_id, const char *message_text) {
    char sql[1024];
    snprintf(sql, sizeof(sql),
             "INSERT INTO messages (message_id, chat_id, sender_id, message_text) "
             "VALUES (%d, %d, %d, '%s');", message_id, chat_id, sender_id, message_text);

    vendor.database.sql.execute_sql(sql);
}

static void edit_message(int message_id, const char *new_message_text) {
    char sql[1024];
    snprintf(sql, sizeof(sql),
             "UPDATE messages SET message_text = '%s' WHERE message_id = %d;",
             new_message_text, message_id);

    vendor.database.sql.execute_sql(sql);
}

static t_messages_struct *get_messages_by_chat_id(int chat_id, int number_of_elements, int page, int *total_messages) {
    int offset = (page - 1) * number_of_elements;

    char sql[1024];
    snprintf(sql, sizeof(sql),
             "SELECT message_id, chat_id, sender_id, message_text, timestamp, read_at, edited_at "
             "FROM messages WHERE chat_id = %d "
             "ORDER BY timestamp DESC "
             "LIMIT %d OFFSET %d;", chat_id, number_of_elements, offset);

    char **results = NULL;
    int rows, cols;
    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    if (rc != 0 || rows == 0) {
        printf("No messages found for chat_id: %d\n", chat_id);
        return NULL;
    }

    if (total_messages != NULL) {
        char count_sql[512];
        snprintf(count_sql, sizeof(count_sql),
                 "SELECT COUNT(*) FROM messages WHERE chat_id = %d;", chat_id);

        char **count_results = NULL;
        int count_rows, count_cols;
        rc = vendor.database.sql.execute_query(count_sql, &count_results, &count_rows, &count_cols);

        if (rc == 0 && count_rows > 0) {
            *total_messages = atoi(count_results[1]);
        }
    }

    t_messages_struct *messages = malloc(sizeof(t_messages_struct) * rows);
    if (messages == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    for (int i = 1; i <= rows; ++i) {
        t_messages_struct *msg = &messages[i - 1];

        msg->message_id = atoi(results[i * cols]);  // message_id
        msg->chat_struct = vendor.database.tables.chats_table.get_chat_by_id(chat_id);
        msg->sender_struct = vendor.database.tables.users_table.get_user_by_id(chat_id);
//        msg->chat_struct.chat_id = atoi(results[i * cols + 1]);  // chat_id
//        msg->sender_struct.user_id = atoi(results[i * cols + 2]);  // sender_id
        msg->message_text = vendor.helpers.strdup(results[i * cols + 3]);  // message_text


        time_t timestamp = (time_t)(atoll(results[i * cols + 4]));
        localtime_r(&timestamp, &msg->timestamp);

        if (results[i * cols + 5] != NULL) {
            time_t read_at_timestamp = (time_t)(atoll(results[i * cols + 5]));
            localtime_r(&read_at_timestamp, &msg->read_at);
        }
    }

    return messages;
}

static void free_struct(t_messages_struct *message) {
    if (message != NULL) {
        if (message->chat_struct != NULL) {
            vendor.database.tables.chats_table.free_struct(message->chat_struct);
        }
        if (message->sender_struct != NULL) {
            vendor.database.tables.users_table.free_struct(message->sender_struct);
        }

        if (message->message_text != NULL) {
            free(message->message_text);
        }

        free(message);
    }
}

t_messages_table init_messages_table(void) {
    t_messages_table table = {
        .create_table = create_table,
        .add_message = add_message,
        .edit_message = edit_message,
        .get_messages_by_chat_id = get_messages_by_chat_id,
        .free_struct = free_struct,
    };

    return table;
}
