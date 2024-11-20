#include "../../inc/header.h"

char *get_group_name_by_chat_id(int chat_id) {
    char sql[256];
    snprintf(sql, sizeof(sql), "SELECT group_name FROM group_info WHERE group_id = %d", chat_id);

    printf("SQL Query: %s\n", sql);

    char **results = NULL;
    int rows, cols;

    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    if (rc != 0) {
        printf("[ERROR] Ошибка при выполнении SQL-запроса: %s\n", sqlite3_errmsg(vendor.database.db));
        return vendor.helpers.strdup("Неизвестная группа");
    }

    printf("Rows: %d, Cols: %d\n", rows, cols);

    if (rows == 0) {
        printf("[INFO] Группа не найдена. Возвращаем значение по умолчанию.\n");
        return vendor.helpers.strdup("Неизвестная группа");
    }

    // В вашем случае, предполагаем что на первом месте находится имя столбца.
    // Получаем результат из первой строки, первой колонки
    printf("Group Name: %s\n", results[cols]);  // Первая строка, первое значение в ней.

    return vendor.helpers.strdup(results[cols]);  // Возвращаем правильное значение
}
