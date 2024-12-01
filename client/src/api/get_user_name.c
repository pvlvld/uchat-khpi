#include "../../inc/header.h"

char *get_user_name(int user_id) {
    const char *sql = "SELECT username FROM users WHERE user_id = ?";
    sqlite3_stmt *stmt;
    char *user_name = NULL;

    int rc = sqlite3_prepare_v2(vendor.database.db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to prepare a request: %s\n", sqlite3_errmsg(vendor.database.db));
        return vendor.helpers.strdup("Unknown user");
    }

    rc = sqlite3_bind_int(stmt, 1, user_id);
    if (rc != SQLITE_OK) {
        printf("[ERROR] Failed to bind the parameter: %s\n", sqlite3_errmsg(vendor.database.db));
        sqlite3_finalize(stmt);
        return vendor.helpers.strdup("Unknown user");
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const char *username = (const char *)sqlite3_column_text(stmt, 0);
        user_name = vendor.helpers.strdup(username ? username : "Unknown user");
    } else {
        user_name = vendor.helpers.strdup("Unknown user");
    }

    sqlite3_finalize(stmt);

    return user_name;
}
