#include "../../inc/header.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

static void create_database_impl(void) {
    int rc = sqlite3_open(vendor.database.db_name, &vendor.database.db);

    if (rc != SQLITE_OK) {
        vendor.database.close_database();
        return;
    }

    vendor.database.tables.users_table.create_table();
    vendor.database.tables.active_users_table.create_table();
    vendor.database.tables.chats_table.create_table();
    vendor.database.tables.personal_chats_table.create_table();
    vendor.database.tables.group_chats_table.create_table();
    vendor.database.tables.group_chat_members_table.create_table();
    vendor.database.tables.messages_table.create_table();
    vendor.database.tables.group_info_table.create_table();
}

static void close_database(void) {
    if (vendor.database.db) {
        sqlite3_close(vendor.database.db);
    }
}

static t_database_tables init_database_tables(void) {
    t_database_tables tables = {
        .users_table = init_users_table(),
        .active_users_table = init_active_users_table(),
        .chats_table = init_chats_table(),
        .messages_table = init_messages_table(),
        .personal_chats_table = init_personal_chats_table(),
        .group_chats_table = init_group_chats_table(),
        .group_chat_members_table = init_group_chat_members_table(),
        .group_info_table = init_group_info_table(),
    };

    return tables;
}

static void ensure_db_directory_exists(void) {
    const char *db_dir = "db";

    if (access(db_dir, F_OK) == -1) {
        if (mkdir(db_dir, 0755) != 0) {
            perror("Failed to create directory 'db'");
            return;
        }
    }
}

t_database init_database(void) {
    ensure_db_directory_exists();

    t_database database = {
        .create_database = create_database_impl,
        .close_database = close_database,
        .db = NULL,
        .sql = init_sql(),
        .tables = init_database_tables(),
    };
    return database;
}
