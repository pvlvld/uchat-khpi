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

static t_users_struct **get_users_from_personal_chats(int *user_count, int chat_id) {
    if (vendor.current_user.user_id == 0) {
        printf("[ERROR] Current user ID is not set.\n");
        return NULL;
    }

    const char *sql =
        "SELECT DISTINCT u.user_id, u.username, u.user_login, u.about, u.is_online, u.public_key, u.updated_at "
        "FROM personal_chats pc "
        "JOIN users u ON (pc.user1_id = u.user_id OR pc.user2_id = u.user_id) "
        "WHERE u.user_id != ? AND (pc.user1_id = ? OR pc.user2_id = ?) "
        "AND NOT EXISTS ("
        "    SELECT 1 FROM group_chat_members gcm "
        "    WHERE gcm.chat_id = ? AND gcm.user_id = u.user_id"
        ");";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(vendor.database.db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[ERROR] Failed to prepare SQL statement: %s\n", sqlite3_errmsg(vendor.database.db));
        return NULL;
    }

    sqlite3_bind_int(stmt, 1, vendor.current_user.user_id);
    sqlite3_bind_int(stmt, 2, vendor.current_user.user_id);
    sqlite3_bind_int(stmt, 3, vendor.current_user.user_id);
    sqlite3_bind_int(stmt, 4, chat_id);

    t_users_struct **users = NULL;
    int count = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        t_users_struct **temp = realloc(users, (count + 1) * sizeof(t_users_struct *));
        if (temp == NULL) {
            printf("[ERROR] Memory allocation failed.\n");
            for (int i = 0; i < count; i++) {
                vendor.database.tables.users_table.free_struct(users[i]);
            }
            free(users);
            sqlite3_finalize(stmt);
            return NULL;
        }
        users = temp;

        t_users_struct *user = (t_users_struct *)malloc(sizeof(t_users_struct));
        if (user == NULL) {
            printf("[ERROR] Memory allocation failed.\n");
            for (int i = 0; i < count; i++) {
                vendor.database.tables.users_table.free_struct(users[i]);
            }
            free(users);
            sqlite3_finalize(stmt);
            return NULL;
        }

        user->user_id = sqlite3_column_int(stmt, 0);
        user->username = vendor.helpers.strdup((const char *)sqlite3_column_text(stmt, 1));
        user->user_login = vendor.helpers.strdup((const char *)sqlite3_column_text(stmt, 2));
        user->about = sqlite3_column_text(stmt, 3) ? vendor.helpers.strdup((const char *)sqlite3_column_text(stmt, 3)) : NULL;
        user->is_online = sqlite3_column_int(stmt, 4);
        user->public_key = vendor.helpers.strdup((const char *)sqlite3_column_text(stmt, 5));

        time_t timestamp = (time_t)sqlite3_column_int64(stmt, 6);
        localtime_r(&timestamp, &user->updated_at);

        users[count] = user;
        count++;
    }

    sqlite3_finalize(stmt);

    *user_count = count;
    return users;
}

static int create_personal_chat(int chat_id, int user2_id) {
    if (vendor.current_user.user_id == 0) {
        printf("[ERROR] Current user ID is not set.\n");
        return -1;
    }

    const char *sql =
        "INSERT INTO personal_chats (chat_id, user1_id, user2_id) "
        "VALUES (?, ?, ?);";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(vendor.database.db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[ERROR] Failed to prepare SQL statement: %s\n", sqlite3_errmsg(vendor.database.db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, chat_id);
    sqlite3_bind_int(stmt, 2, vendor.current_user.user_id);
    sqlite3_bind_int(stmt, 3, user2_id);

    int result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        printf("[ERROR] Failed to execute SQL statement: %s\n", sqlite3_errmsg(vendor.database.db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

static int get_personal_chat_with_user(int user_id) {
    if (vendor.current_user.user_id == 0) {
        printf("[ERROR] Current user ID is not set.\n");
        return 0;
    }

    const char *sql =
        "SELECT chat_id "
        "FROM personal_chats "
        "WHERE (user1_id = ? AND user2_id = ?) "
        "   OR (user1_id = ? AND user2_id = ?);";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(vendor.database.db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        printf("[ERROR] Failed to prepare SQL statement: %s\n", sqlite3_errmsg(vendor.database.db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, vendor.current_user.user_id);
    sqlite3_bind_int(stmt, 2, user_id);
    sqlite3_bind_int(stmt, 3, user_id);
    sqlite3_bind_int(stmt, 4, vendor.current_user.user_id);

    int chat_id = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        chat_id = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return chat_id;
}

t_personal_chats_table init_personal_chats_table(void) {
    t_personal_chats_table table = {
        .create_table = create_table,
        .get_users = get_users_from_personal_chats,
        .create_personal_chat = create_personal_chat,
        .get_personal_chat_with_user = get_personal_chat_with_user,
    };

    return table;
}
