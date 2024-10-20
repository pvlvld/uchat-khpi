#include "../../inc/database.h"
#include "../../inc/header.h"

t_database init_database(void) {
    t_database database = {
        .db_name = vendor.env.db_name,
        .create_database = _create_database_impl,
        .close_database = _close_database,
        .db = NULL,
        .users_table = init_users_table(),
    };
    return database;
}

void find_user_by_id(int user_id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT * FROM users WHERE id = %d;", user_id);
    print_column_value(sql, "id");
}

// Implementation of the function to create a database
void _create_database_impl(void) {
    int rc = sqlite3_open(vendor.database.db_name, &vendor.database.db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(vendor.database.db));
        vendor.database.close_database();
        return;
    }

   if (vendor.env.dev_mode) printf("Database created successfully\n");

    // Creating the users table
    vendor.database.users_table.create_table();
    // find_user_by_id(1);
}

void _close_database(void) {
    if (vendor.database.db) {
        sqlite3_close(vendor.database.db);
    }
}
