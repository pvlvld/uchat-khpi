#include "../../inc/header.h"

// Функция для удаления сообщения из базы данных и с сервера
static int delete_message(int message_id) {
    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "/messages/%d", message_id);

    // Симуляция запроса на сервер для удаления
    int server_response = vendor.server_requests.delete_data(endpoint);
    if (server_response != 1) {
        printf("Failed to delete message from server\n");
        return -1;
    }

    // Если сервер ответил успешно, удаляем сообщение из базы данных
    char sql[512];
    snprintf(sql, sizeof(sql), "DELETE FROM messages WHERE message_id = %d;", message_id);
    vendor.database.sql.execute_sql(sql);

    return 0; // Успех
}

// Функция для редактирования сообщения в базе данных и на сервере
static int edit_message(int message_id, const char *new_message_text) {
    char endpoint[256];
    snprintf(endpoint, sizeof(endpoint), "/messages/%d", message_id);

    char payload[1024];
    snprintf(payload, sizeof(payload), "{\"message_text\": \"%s\"}", new_message_text);

    // Симуляция запроса на сервер для редактирования
    int server_response = vendor.server_requests.post_data(endpoint, payload);
    if (server_response != 1) {
        printf("Failed to update message on server\n");
        return -1;
    }

    // Если сервер ответил успешно, обновляем сообщение в базе данных
    char sql[1024];
    snprintf(sql, sizeof(sql),
             "UPDATE messages SET message_text = '%s', edited_at = strftime('%%s', 'now') "
             "WHERE message_id = %d;",
             new_message_text, message_id);
    vendor.database.sql.execute_sql(sql);

    return 0; // Успех
}

// Инициализация слоя для редактирования и удаления
t_edit_delete_layer init_edit_delete_layer(void) {
    t_edit_delete_layer layer = {
        .delete_message = delete_message,
        .edit_message = edit_message,
    };
    return layer;
}
