#include "../../../inc/header.h"

static void create_table(void) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS group_chat_members ("
        "chat_id INTEGER NOT NULL, "
        "user_id INTEGER NOT NULL, "
        "role TEXT NOT NULL CHECK (role IN ('member', 'admin', 'owner', 'left', 'banned', 'restricted')), "
        "PRIMARY KEY (chat_id, user_id), "
        "FOREIGN KEY (chat_id) REFERENCES group_chats(chat_id) ON DELETE CASCADE, "
        "FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE"
        ");";
    vendor.database.sql.execute_sql(sql);
}

t_group_chat_members_table init_group_chat_members_table(void) {
    t_group_chat_members_table table = {
        .create_table = create_table,
    };

    return table;
}
