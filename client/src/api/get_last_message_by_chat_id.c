#include "../../inc/header.h"

char *get_last_message_by_chat_id(int chat_id, int *sender_id) {
    char sql[256];
    snprintf(sql, sizeof(sql),
        "SELECT message_text, sender_id FROM messages WHERE chat_id = %d ORDER BY timestamp DESC LIMIT 1", chat_id);

    char **results = NULL;
    int rows, cols;

    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);
    if (rc != 0) {
        printf("[ERROR] Error during SQL query execution: %s\n", sqlite3_errmsg(vendor.database.db));
        return vendor.helpers.strdup("Message received error");
    }

    if (rows == 0) {
        if (vendor.debug_mode >= 1) printf("[INFO] No messages found.\n");
        return vendor.helpers.strdup("No messages");
    }
    if (vendor.debug_mode == 1) {
        printf("[DEBUG] Results obtained:\n");
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("[DEBUG] results[%d][%d]: %s\n", i, j, results[i * cols + j]);
            }
        }
    }

    char *message_text = results[cols];
    *sender_id = atoi(results[cols + 1]);

    if (vendor.debug_mode == 1) {
        printf("[DEBUG] Last message: %s\n", message_text);
        printf("[DEBUG] Sender ID: %d\n", *sender_id);
    }

    char *decrypt = vendor.crypto.decrypt_data_from_db(message_text);

    return decrypt;
}
