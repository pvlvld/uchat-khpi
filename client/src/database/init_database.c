#include "../../inc/header.h"

static void create_database_impl(void) {
    int rc = sqlite3_open(vendor.database.db_name, &vendor.database.db);

    if (rc != SQLITE_OK) {
        vendor.database.close_database();
        return;
    }

    vendor.database.tables.users_table.create_table();
    vendor.database.tables.chats_table.create_table();
    vendor.database.tables.personal_chats_table.create_table();
    vendor.database.tables.group_chats_table.create_table();
    vendor.database.tables.group_chat_members_table.create_table();
    vendor.database.tables.messages_table.create_table();
}

static void close_database(void) {
    if (vendor.database.db) {
        sqlite3_close(vendor.database.db);
    }
}

static t_database_tables init_database_tables(void) {
    t_database_tables tables = {
        .users_table = init_users_table(),
        .chats_table = init_chats_table(),
        .messages_table = init_messages_table(),
        .personal_chats_table = init_personal_chats_table(),
        .group_chats_table = init_group_chats_table(),
        .group_chat_members_table = init_group_chat_members_table(),
    };

    return tables;
}

t_database init_database(void) {
    t_database database = {
        .db_name = "ucaht.db",
        .create_database = create_database_impl,
        .close_database = close_database,
        .db = NULL,
        .sql = init_sql(),
        .tables = init_database_tables(),
    };
    return database;
}
