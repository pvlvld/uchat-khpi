#include "../../../inc/header.h"

static void create_table(void) {
    if (vendor.database.db == NULL) {
        printf("[ERROR] База данных не инициализирована!\n");
        return;
    }

    const char *sql =
        "CREATE TABLE IF NOT EXISTS group_info ("
        "group_id INTEGER PRIMARY KEY, "
        "group_name TEXT, "
        "group_picture INTEGER DEFAULT NULL, "
        "FOREIGN KEY (group_id) REFERENCES chats(chat_id) ON DELETE CASCADE, "
        "FOREIGN KEY (group_picture) REFERENCES media(media_id) ON DELETE SET NULL"
        ");";

    vendor.database.sql.execute_sql(sql);
    printf("[INFO] Таблица group_info создана (или уже существует).\n");
}

t_group_chats_table init_group_chats_table(void) {
    t_group_chats_table table = {
        .create_table = create_table,
    };

    return table;
}
