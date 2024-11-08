#include "../../../inc/header.h"

static void create_table(void) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS personal_chats ("
        "chat_id INTEGER PRIMARY KEY, "
        "user1_id INTEGER NOT NULL, "
        "user2_id INTEGER NOT NULL, "
        "background INTEGER DEFAULT NULL, "
        "UNIQUE (user1_id, user2_id), "
        "FOREIGN KEY (chat_id) REFERENCES chats(chat_id) ON DELETE CASCADE, "
        "FOREIGN KEY (user1_id) REFERENCES users(user_id) ON DELETE CASCADE, "
        "FOREIGN KEY (user2_id) REFERENCES users(user_id) ON DELETE CASCADE"
        ");";
    vendor.database.sql.execute_sql(sql);
}

t_personal_chats_table init_personal_chats_table(void) {
    t_personal_chats_table table = {
        .create_table = create_table,
    };

    return table;
}
