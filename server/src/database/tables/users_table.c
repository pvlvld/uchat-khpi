#include "../../../inc/database.h"
#include "../../../inc/header.h"

t_users_table init_users_table(void) {
    t_users_table table = {
        .create_table = _create_users_table,
        .get_or_create = _get_or_create_user_id,
        .find_by_phone = _find_by_phone,
    };

    return table;
}

// Function for creating the users table
void _create_users_table(void) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT, "
        "phone_number TEXT NOT NULL UNIQUE, "
        "ava_url TEXT, "
        "created_at TIMESTAMP DEFAULT (strftime('%s', 'now')), "
        "updated_at DATETIME"
        ");";
    sqlite.execute_sql(sql);
}

unsigned long _get_or_create_user_id(const char *phone_number) {
    char *sql;
    char *result;
    int user_id;

    // Query to verify an existing user
    sql = sqlite3_mprintf("SELECT id FROM users WHERE phone_number = '%q'", phone_number);
    result = vendor.sql.get_column_value(sql, "id");

    if (result) {
        // User found, return ID
        user_id = atoi(result);
        free(result);
        sqlite3_free(sql);
        return user_id;
    }
    free(result);

    // Form SQL query for inserting a new user
    sql = sqlite3_mprintf(
        "INSERT INTO users (phone_number) VALUES ('%q')",
        phone_number
    );
    execute_sql(sql);

    // Get the ID of the newly inserted record
    user_id = (int)sqlite3_last_insert_rowid(vendor.database.db);
    sqlite3_free(sql);

    return user_id;
}

unsigned long _find_by_phone(const char *phone_number) {
    char *sql;
    char *result;
    int user_id;

    // Query to verify an existing user
    sql = sqlite3_mprintf("SELECT id FROM users WHERE phone_number = '%q'", phone_number);
    result = vendor.sql.get_column_value(sql, "id");

    if (result) {
        // User found, return ID
        user_id = atoi(result);
        free(result);
        sqlite3_free(sql);
        return user_id;
    }
    free(result);

    return -1;
}
