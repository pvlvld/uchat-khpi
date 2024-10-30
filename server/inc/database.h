#ifndef DATABASE_H
#define DATABASE_H

#pragma once

#include "sql.h"
#include "../../libraries/cJSON/inc/cJSON.h"

typedef struct {
    void (*create_table)(void);
    unsigned long (*get_or_create)(const char *phone_number);
    unsigned long (*find_by_phone)(const char *phone_number);
} t_users_table;
void _create_users_table(void);
unsigned long _get_or_create_user_id(const char *phone_number) ;
unsigned long _find_by_phone(const char *phone_number);
t_users_table init_users_table(void);

typedef struct {
    char* db_name;
    void (*create_database)(void);
    void (*close_database)(void);
    t_users_table users_table;
    sqlite3 *db;
} t_database;

// Function for creating a database
void _create_database_impl(void);
void _close_database(void);

void execute_sql(const char *sql);
int query_callback(void *data, int argc, char **argv, char **azColName);
int execute_query(const char *sql, char ***results, int *rows, int *cols);
void print_query_results(const char *sql);
void print_column_value(const char *sql, const char *column_name);

t_database init_database(void);

#endif //DATABASE_H
