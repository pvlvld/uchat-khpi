#include "../../inc/header.h"

int get_other_user_id(int chat_id) {
    const char *sql = "SELECT user1_id, user2_id FROM personal_chats WHERE chat_id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(vendor.database.db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("[ERROR] Не удалось подготовить запрос: %s\n", sqlite3_errmsg(vendor.database.db));
        return -1;
    }

    // Привязка параметра chat_id
    sqlite3_bind_int(stmt, 1, chat_id);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int user1_id = sqlite3_column_int(stmt, 0);
        int user2_id = sqlite3_column_int(stmt, 1);
        sqlite3_finalize(stmt);

        // Возвращаем ID другого пользователя
        if (user1_id == vendor.current_user.user_id) {
            return user2_id;
        } else if (user2_id == vendor.current_user.user_id) {
            return user1_id;
        } else {
            printf("[ERROR] Не удалось найти другого пользователя для чата ID %d.\n", chat_id);
            return -1;
        }
    } else {
        printf("[ERROR] Не удалось получить данные о пользователе для чата ID %d.\n", chat_id);
        sqlite3_finalize(stmt);
        return -1;
    }
}
