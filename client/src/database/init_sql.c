#include "../../inc/header.h"

static void execute_sql(const char *sql) {
    char *errmsg = NULL;
    int rc = sqlite3_exec(vendor.database.db, sql, 0, 0, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
    }
}

static int execute_query(const char *sql, char ***results, int *rows, int *cols) {
    char *errmsg = NULL;
    int rc;

    rc = sqlite3_get_table(vendor.database.db, sql, results, rows, cols, &errmsg);
    if (rc != SQLITE_OK) {
        sqlite3_free(errmsg);
        return rc;
    }

    return SQLITE_OK;
}

static char *get_column_value(const char *sql, const char *column_name) {
    char **results;
    int rows, cols;
    int rc;
    char *value = NULL;

    rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);
    if (rc != SQLITE_OK) {
        return NULL;
    }

    int col_index = -1;
    for (int i = 0; i < cols; i++) {
        if (strcmp(results[i], column_name) == 0) {
            col_index = i;
            break;
        }
    }

    if (col_index == -1) {
        sqlite3_free_table(results);
        return NULL;
    }

    if (rows > 0 && col_index >= 0) {
        value = vendor.helpers.strdup(results[cols + col_index]);
    }

    sqlite3_free_table(results);
    return value; // Caller is responsible for freeing with free.
}

t_sql init_sql(void) {
    t_sql sql = {
        .execute_sql = execute_sql,
        .execute_query = execute_query,
        .get_column_value = get_column_value,
        .free = sqlite3_free,
    };

    return sql;
}
