#include "../../../inc/header.h"

static void create_table(void) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS group_chats ("
        "chat_id INTEGER PRIMARY KEY, "
        "group_name TEXT, "
        "group_picture INTEGER DEFAULT NULL, "
        "background INTEGER DEFAULT NULL, "
        "FOREIGN KEY (chat_id) REFERENCES chats(chat_id) ON DELETE CASCADE, "
        "FOREIGN KEY (group_picture) REFERENCES media(media_id) ON DELETE SET NULL, "
        "FOREIGN KEY (background) REFERENCES media(media_id) ON DELETE SET NULL"
        ");";
    vendor.database.sql.execute_sql(sql);
}

t_group_chats_table init_group_chats_table(void) {
    t_group_chats_table table = {
        .create_table = create_table,
    };

    return table;
}
