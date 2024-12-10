#include "../../../inc/header.h"

static void create_table(void) {
    if (vendor.database.db == NULL) {
        printf("[ERROR] Database not initialized!\n\n");
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
}

static int create_group(int group_id, const char *group_name) {
    vendor.database.tables.chats_table.add_chat(group_id, "group");

    if (vendor.database.db == NULL) {
        printf("[ERROR] Database not initialized!\n\n");
        return -1;
    }

    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO group_info (group_id, group_name) VALUES (?, ?);";

    if (sqlite3_prepare_v2(vendor.database.db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[ERROR] Failed to prepare SQL statement: %s\n", sqlite3_errmsg(vendor.database.db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, group_id);
    sqlite3_bind_text(stmt, 2, group_name, -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("[ERROR] Failed to insert group: %s\n", sqlite3_errmsg(vendor.database.db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);

    return group_id;
}

t_group_chats_table init_group_chats_table(void) {
    t_group_chats_table table = {
        .create_table = create_table,
        .create_group = create_group,
    };

    return table;
}
