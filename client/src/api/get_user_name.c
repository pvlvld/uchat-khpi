#include "../../inc/header.h"

char *get_user_name(int user_id) {
    const char *sql = "SELECT username FROM users WHERE user_id = ?";
    sqlite3_stmt *stmt;
    char *user_name = NULL;

    // Подготовка SQL-запроса
    int rc = sqlite3_prepare_v2(vendor.database.db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("[ERROR] Не удалось подготовить запрос: %s\n", sqlite3_errmsg(vendor.database.db));
        return vendor.helpers.strdup("Неизвестный пользователь");
    }

    // Привязка параметра user_id к запросу
    rc = sqlite3_bind_int(stmt, 1, user_id);  // Привязываем user_id к первому параметру
    if (rc != SQLITE_OK) {
        printf("[ERROR] Не удалось привязать параметр: %s\n", sqlite3_errmsg(vendor.database.db));
        sqlite3_finalize(stmt);
        return vendor.helpers.strdup("Неизвестный пользователь");
    }

    // Выполнение запроса и получение результатов
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // Извлекаем имя пользователя из результата запроса
        const char *username = (const char *)sqlite3_column_text(stmt, 0);
        user_name = vendor.helpers.strdup(username ? username : "Неизвестный пользователь");
    } else {
        user_name = vendor.helpers.strdup("Неизвестный пользователь");
    }

    // Освобождаем ресурсы
    sqlite3_finalize(stmt);

    return user_name;
}
