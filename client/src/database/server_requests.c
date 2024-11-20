#include "../../inc/header.h"

// Функция для получения данных с сервера (псевдозапрос)
static int fetch_data_from_server(const char *endpoint) {
    printf("Fetching data from server: %s\n", endpoint);
    // Симуляция получения данных
    return 1; // Успешный ответ
}

// Функция для отправки данных на сервер (псевдозапрос)
static int post_data_to_server(const char *endpoint, const char *payload) {
    printf("Posting data to server: %s\nPayload: %s\n", endpoint, payload);
    // Симуляция отправки данных на сервер
    return 1; // Успешный ответ
}

// Функция для удаления данных с сервера (псевдозапрос)
static int delete_data_on_server(const char *endpoint) {
    printf("Deleting data on server: %s\n", endpoint);
    // Симуляция удаления данных на сервере
    return 1; // Успешный ответ
}

// Инициализация структуры для работы с запросами
t_server_requests init_server_requests(void) {
    t_server_requests requests = {
        .fetch_data = fetch_data_from_server,
        .post_data = post_data_to_server,
        .delete_data = delete_data_on_server,
    };
    return requests;
}

// Функция для получения сообщений из базы данных с пагинацией и сортировкой
t_messages_struct *get_messages_from_db(int chat_id, int page, int page_size, const char *sort_by, const char *order, int *total_messages) {
    return vendor.database.tables.messages_table.get_messages_by_chat_id(chat_id, sort_by, order, page_size, page, total_messages);
}
