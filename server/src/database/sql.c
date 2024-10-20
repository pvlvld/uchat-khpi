#include "../../inc/sql.h"
#include "../../inc/header.h"

t_sql sqlite = {
    .execute_sql = execute_sql,
    .query_callback = query_callback,
    .execute_query = execute_query,
    .get_column_value = get_column_value
};

// Function for executing SQL query
void execute_sql(const char *sql) {
    char *errmsg = NULL;
    int rc = sqlite3_exec(vendor.database.db, sql, 0, 0, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
    } else {
        if (vendor.env.dev_mode) printf("SQL executed successfully: %s\n", sql);
    }
}

int query_callback(void *data, int argc, char **argv, char **azColName) {
    // Callback function for processing request results
    char **results = (char **)data;
    int *current_index = (int *)results;
    int i;

    for (i = 0; i < argc; i++) {
        if (vendor.env.dev_mode) printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }

    (*current_index)++;
    return 0;
}

// Function for executing a general SQL query
int execute_query(const char *sql, char ***results, int *rows, int *cols) {
    char *errmsg = NULL;
    int rc;

    rc = sqlite3_get_table(vendor.database.db, sql, results, rows, cols, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return rc;
    }

    return SQLITE_OK;
}

// Function to execute an SQL query and get the value from the specified column
char *get_column_value(const char *sql, const char *column_name) {
    char **results;
    int rows, cols;
    int rc;
    char *value = NULL;

    rc = sqlite.execute_query(sql, &results, &rows, &cols);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute query: %s\n", sql);
        return NULL;
    }

    // Find the column index
    int col_index = -1;
    for (int i = 0; i < cols; i++) {
        if (strcmp(results[i], column_name) == 0) {
            col_index = i;
            break;
        }
    }

    if (col_index == -1) {
        if (vendor.env.dev_mode) fprintf(stderr, "Column %s not found in query results\n", column_name);
        sqlite3_free_table(results);
        return NULL;
    }

    // Get the value from the first line of data
    if (rows > 0 && col_index >= 0) {
        // Use malloc and strcpy instead of strdup
        value = malloc(strlen(results[cols + col_index]) + 1);
        if (value != NULL) {
            strcpy(value, results[cols + col_index]); // Read the value from the first line
        }
    }

    sqlite3_free_table(results);
    return value;
}

// Example of using the function
void print_column_value(const char *sql, const char *column_name) {
    char *value = sqlite.get_column_value(sql, column_name);
    if (value) {
        if (vendor.env.dev_mode) printf("Value of column %s: %s\n", column_name, value);
        free(value);
    } else {
        if (vendor.env.dev_mode) printf("Failed to get value for column %s\n", column_name);
    }
}

// Example of using the function
void print_query_results(const char *sql) {
    char **results;
    int rows, cols;
    int rc;

    rc = sqlite.execute_query(sql, &results, &rows, &cols);
    if (rc == SQLITE_OK) {
        for (int i = 0; i <= rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (vendor.env.dev_mode) printf("%s\t", results[i * cols + j]);
            }
            if (vendor.env.dev_mode) printf("\n");
        }
        sqlite3_free_table(results);
    } else {
        fprintf(stderr, "Failed to execute query: %s\n", sql);
    }
}
