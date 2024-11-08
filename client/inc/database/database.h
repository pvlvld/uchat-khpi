#ifndef DATABASE_H
#define DATABASE_H

#include "../header.h"
#include "chats_table.h"
#include "users_table.h"
#include "messages_table.h"
#include "personal_chats_table.h"
#include "group_chats_table.h"
#include "group_chat_members_table.h"

typedef struct {
    void (*execute_sql)(const char *sql);
    int (*execute_query)(const char *sql, char ***results, int *rows, int *cols);
    char *(*get_column_value)(const char *sql, const char *column_name);
    void (*free)(void*);
} t_sql;
t_sql init_sql(void);

typedef struct {
    t_users_table users_table;
    t_chats_table chats_table;
    t_messages_table messages_table;
    t_personal_chats_table personal_chats_table;
    t_group_chats_table group_chats_table;
    t_group_chat_members_table group_chat_members_table;
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
