#ifndef DATABASE_H
#define DATABASE_H

#include "../header.h"
#include "chats_table.h"
#include "users_table.h"
#include "active_users_table.h"
#include "messages_table.h"
#include "personal_chats_table.h"
#include "group_chats_table.h"
#include "group_chat_members_table.h"
#include "group_info_table.h"

typedef struct {
    unsigned int id;
    char *name;
    t_messages_struct *last_message;
    char *sender_name;
    t_chats_types type;
    char *path_to_logo;
    int unreaded_messages;
    time_t timestamp;
    char *another_user_public_key;
} t_chat_info;

typedef struct {
    void (*execute_sql)(const char *sql);
    int (*execute_query)(const char *sql, char ***results, int *rows, int *cols);
    char *(*get_column_value)(const char *sql, const char *column_name);
    void (*free)(void*);
} t_sql;
t_sql init_sql(void);

typedef struct {
    t_users_table users_table;
    t_active_users_table active_users_table;
    t_chats_table chats_table;
    t_messages_table messages_table;
    t_personal_chats_table personal_chats_table;
    t_group_chats_table group_chats_table;
    t_group_chat_members_table group_chat_members_table;
    t_group_info_table group_info_table;
} t_database_tables;

typedef struct {
    t_sql sql;
    char *db_name;
    sqlite3 *db;
    void (*create_database)(void);
    void (*close_database)(void);
    t_database_tables tables;
} t_database;
t_database init_database(void);

#endif //DATABASE_H
