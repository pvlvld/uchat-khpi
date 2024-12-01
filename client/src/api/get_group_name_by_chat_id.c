#include "../../inc/header.h"

char *get_group_name_by_chat_id(int chat_id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT group_name FROM group_info WHERE group_id = %d", chat_id);

    char **results = NULL;
    int rows, cols;

    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    if (rc != 0) {
        printf("[ERROR] Error during SQL query execution: %s\n", sqlite3_errmsg(vendor.database.db));
        return vendor.helpers.strdup("Unknown group");
    }

    if (rows == 0) {
       if (vendor.debug_mode == 1) printf("[DEBUG] Group not found. Return the default value.\n");
        return vendor.helpers.strdup("Unknown group");
    }
    return vendor.helpers.strdup(results[cols]);
}
