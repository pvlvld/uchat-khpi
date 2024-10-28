#ifndef SQL_H
#define SQL_H

#pragma once

#include "../../libraries/sqlite/include/sqlite3.h"
#include "sqlite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void (*execute_sql)(const char *sql);
    int (*query_callback)(void *data, int argc, char **argv, char **azColName);
    int (*execute_query)(const char *sql, char ***results, int *rows, int *cols);
    char *(*get_column_value)(const char *sql, const char *column_name);
} t_sql;

void execute_sql(const char *sql);
int query_callback(void *data, int argc, char **argv, char **azColName);
int execute_query(const char *sql, char ***results, int *rows, int *cols);
char *get_column_value(const char *sql, const char *column_name);

extern t_sql sqlite;

#endif //SQL_H
