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

static t_users_struct *get_user_by_id(int user_id) {
    char sql[512];
    snprintf(sql, sizeof(sql),
             "SELECT user_id, username, user_login, about, is_online, public_key, updated_at FROM users WHERE user_id = %d;",
             user_id);

    char **results = NULL;
    int rows, cols;
    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    if (rc != 0 || rows == 0) {
        printf("No results found for user_id: %d\n", user_id);
        return NULL;
    }

    t_users_struct *user = (t_users_struct *)malloc(sizeof(t_users_struct));
    if (user == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    user->user_id = atoi(results[cols]);
    user->username = vendor.helpers.strdup(results[cols + 1]);
    user->user_login = vendor.helpers.strdup(results[cols + 2]);
    user->about = results[cols + 3] ? vendor.helpers.strdup(results[cols + 3]) : NULL;
    user->is_online = atoi(results[cols + 4]);
    user->public_key = vendor.helpers.strdup(results[cols + 5]);

    time_t timestamp = (time_t)atoll(results[cols + 6]);
    localtime_r(&timestamp, &user->updated_at);

    return user;
}

static int add_user(t_users_struct *user) {
    if (user == NULL) {
        printf("[ERROR] User structure is NULL.\n");
        return -1;
    }

    const char *sql =
        "INSERT INTO users (user_id, username, user_login, about, is_online, public_key) "
        "VALUES (?, ?, ?, ?, 0, ?);";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(vendor.database.db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[ERROR] Failed to prepare SQL statement: %s\n", sqlite3_errmsg(vendor.database.db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, user->user_id);
    sqlite3_bind_text(stmt, 2, user->username, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user->user_login, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, user->about ? user->about : NULL, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, user->public_key, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("[ERROR] Failed to insert user: %s\n", sqlite3_errmsg(vendor.database.db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

static void free_struct(t_users_struct *user) {
    if (user != NULL) {
        if (user->username != NULL) {
            free(user->username);
        }
        if (user->user_login != NULL) {
            free(user->user_login);
        }
        if (user->about != NULL) {
            free(user->about);
        }
        if (user->public_key != NULL) {
            free(user->public_key);
        }
        free(user);
    }
}

t_users_table init_users_table(void) {
    t_users_table table = {
        .create_table = create_users_table,
        .get_user_by_id = get_user_by_id,
        .free_struct = free_struct,
        .add_user = add_user,
    };

    return table;
}
