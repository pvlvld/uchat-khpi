#include "../../../inc/header.h"

static void create_users_table(void) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "user_id INTEGER PRIMARY KEY NOT NULL, "
        "username TEXT UNIQUE NOT NULL, "
        "user_login TEXT UNIQUE NOT NULL, "
        "about TEXT DEFAULT NULL, "
        "is_online BOOLEAN NOT NULL DEFAULT 0, "  // FALSE defaul
        "public_key TEXT NOT NULL, "
        "updated_at TIMESTAMP DEFAULT (strftime('%s', 'now'))"
        ");";
    vendor.database.sql.execute_sql(sql);
}

t_users_table init_users_table(void) {
    t_users_table table = {
        .create_table = create_users_table,
    };

    return table;
}
