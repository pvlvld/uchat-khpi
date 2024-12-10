#include "../../../inc/header.h"

static void create_table(void) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS group_info ("
        "group_id INTEGER PRIMARY KEY, "
        "group_name TEXT, "
        "group_picture INTEGER DEFAULT NULL, "
        "FOREIGN KEY (group_id) REFERENCES chats(chat_id) ON DELETE CASCADE, "
        "FOREIGN KEY (group_picture) REFERENCES media(media_id) ON DELETE SET NULL"
        ");";
    vendor.database.sql.execute_sql(sql);
}

t_group_info_table init_group_info_table(void) {
    t_group_info_table table = {
        .create_table = create_table,
    };

    return table;
}
