#include "../../inc/header.h"

char *get_last_message_by_chat_id(int chat_id, int *sender_id) {
    // SQL-запрос для получения последнего сообщения по chat_id
    char sql[256];
    snprintf(sql, sizeof(sql),
        "SELECT message_text, sender_id FROM messages WHERE chat_id = %d ORDER BY timestamp DESC LIMIT 1", chat_id);

    printf("[DEBUG] SQL Query: %s\n", sql);  // Выводим SQL-запрос

    char **results = NULL;
    int rows, cols;

    // Выполнение SQL-запроса
    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);
    if (rc != 0) {
        printf("[ERROR] Ошибка при выполнении SQL-запроса: %s\n", sqlite3_errmsg(vendor.database.db));
        return vendor.helpers.strdup("Ошибка получения сообщения");
    }

    // Проверка наличия сообщений
    if (rows == 0) {
        printf("[INFO] Сообщений не найдено.\n");
        return vendor.helpers.strdup("Нет сообщений");
    }

    // Проверяем полученные результаты
    printf("[DEBUG] Получены результаты:\n");
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("[DEBUG] results[%d][%d]: %s\n", i, j, results[i * cols + j]);
        }
    }

    // В результате запроса результаты могут быть сдвинуты на одну строку из-за заголовков, если используется sqlite3_get_table
    // То есть, начинаем с results[1] вместо results[0] для данных.
    char *message_text = results[cols];  // Текст сообщения (первая строка результата)
    *sender_id = atoi(results[cols + 1]);   // ID отправителя

    printf("[DEBUG] Последнее сообщение: %s\n", message_text);
    printf("[DEBUG] ID отправителя: %d\n", *sender_id);

    char *decrypt = vendor.crypto.decrypt_data_from_db(message_text);

    return decrypt;  // Возвращаем текст последнего сообщения
}
