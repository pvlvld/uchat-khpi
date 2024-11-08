#include "../../../inc/header.h"

static void create_table(void) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS messages ("
        "message_id INTEGER PRIMARY KEY, "    // ID, retrieved from the server
        "chat_id INTEGER NOT NULL, "
        "sender_id INTEGER NOT NULL, "
        "message_text TEXT NOT NULL, "
        "timestamp TIMESTAMP DEFAULT (strftime('%s', 'now')), "
        "read_at TIMESTAMP DEFAULT NULL, "
        "FOREIGN KEY (chat_id) REFERENCES chats(chat_id) ON DELETE CASCADE"
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

t_messages_table init_messages_table(void) {
    t_messages_table table = {
        .create_table = create_table,
        .add_message = add_message,
        .edit_message = edit_message,
    };

    return table;
}
