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

static void add_member(int chat_id, int user_id, const char *role) {
    if (vendor.database.db == NULL) {
        printf("[ERROR] Database not initialized!\n\n");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql =
        "INSERT INTO group_chat_members (chat_id, user_id, role) VALUES (?, ?, ?);";

    if (sqlite3_prepare_v2(vendor.database.db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[ERROR] Failed to prepare SQL statement: %s\n", sqlite3_errmsg(vendor.database.db));
        return;
    }

    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_bind_int(stmt, 2, user_id);
    sqlite3_bind_text(stmt, 3, role, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("[ERROR] Failed to add member: %s\n", sqlite3_errmsg(vendor.database.db));
    } else {
        if (vendor.debug_mode >= 1) printf("[INFO] User %d added to chat %d with role '%s'.\n", user_id, chat_id, role);
    }

    sqlite3_finalize(stmt);
}

t_group_chat_members_table init_group_chat_members_table(void) {
    t_group_chat_members_table table = {
        .create_table = create_table,
        .add_member = add_member,
    };

    return table;
}
