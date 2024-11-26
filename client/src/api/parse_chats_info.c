#include "../../inc/header.h"

t_chat_info **parse_chats_info(void) {
    const char *sql = "SELECT chat_id, chat_type, created_at FROM chats ORDER BY created_at DESC;";
    char **results = NULL;
    int rows, cols;
    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    if (rc != 0) {
        printf("[ERROR] Ошибка при выполнении SQL-запроса: %s\n", sqlite3_errmsg(vendor.database.db));
        return NULL;
    }

    if (rows == 0) {
        printf("[INFO] Чаты не найдены. Таблица 'chats' пустая.\n");
        return NULL;
    }

    printf("[DEBUG] Найдено строк: %d, Колонок: %d\n", rows, cols);

    t_chat_info **chats_info = malloc(sizeof(t_chat_info *) * (rows + 1));
    if (!chats_info) {
        printf("[ERROR] Ошибка выделения памяти для массива структур.\n");
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        chats_info[i] = malloc(sizeof(t_chat_info));
        if (!chats_info[i]) {
            printf("[ERROR] Ошибка выделения памяти для структуры t_chat_info.\n");
            for (int j = 0; j < i; j++) {
                free(chats_info[j]);
            }
            free(chats_info);
            return NULL;
        }

        // Получаем данные из результатов запроса
        chats_info[i]->id = atoi(results[(i + 1) * cols]);
        const char *type_str = results[(i + 1) * cols + 1];
        chats_info[i]->type = (strcmp(type_str, "personal") == 0) ? PERSONAL :
                              (strcmp(type_str, "group") == 0) ? GROUP : CHANNEL;
        chats_info[i]->timestamp = (time_t)atoll(results[(i + 1) * cols + 2]);

        printf("[DEBUG] Обрабатываем чат #%d\n", i);
        printf("[DEBUG] ID чата: %d\n", chats_info[i]->id);
        printf("[DEBUG] Тип чата: %d\n", chats_info[i]->type);
        printf("[DEBUG] Timestamp: %ld\n", chats_info[i]->timestamp);

        chats_info[i]->path_to_logo = "logo_2.jpg";
        chats_info[i]->unreaded_messages = 0;

        // Получаем имя чата и последнее сообщение
        if (chats_info[i]->type == PERSONAL) {
            printf("[DEBUG] Это персональный чат\n");
            int other_user_id = get_other_user_id(chats_info[i]->id);
            chats_info[i]->name = get_user_name(other_user_id);
//            int sender_id;
            // Получаем последнее сообщение для персонального чата
//            chats_info[i]->last_message = get_last_message_by_chat_id(chats_info[i]->id, &sender_id);
            chats_info[i]->last_message = vendor.database.tables.messages_table.get_messages_by_chat_id(chats_info[i]->id, 1, 1, NULL);
//            if (!chats_info[i]->last_message) break;
//            chats_info[i]->sender_name = vendor.helpers.strdup("");

        } else if (chats_info[i]->type == GROUP) {
            printf("[DEBUG] Это групповой чат\n");
            chats_info[i]->name = get_group_name_by_chat_id(chats_info[i]->id);
            chats_info[i]->last_message = vendor.database.tables.messages_table.get_messages_by_chat_id(chats_info[i]->id, 1, 1, NULL);
//            if (chats_info[i]->last_message) {
//                chats_info[i]->sender_name = vendor.helpers.strdup(chats_info[i]->last_message->sender_struct->username);
//            } else {
//                chats_info[i]->sender_name = "";
//            }

//            int sender_id;
            // Получаем последнее сообщение и имя отправителя для группового чата
//            chats_info[i]->last_message = vendor.database.tables.messages_table.get_messages_by_chat_id(chats_info[i]->id, 1, 1, NULL);
//            if (sender_id > 0) {
//                chats_info[i]->sender_name = get_user_name(sender_id);  // Получаем имя отправителя
//            } else {
//                chats_info[i]->sender_name = vendor.helpers.strdup("Неизвестный отправитель");
//            }
        } else {
            chats_info[i]->name = vendor.helpers.strdup("Неизвестный чат");
            chats_info[i]->last_message->message_text = vendor.helpers.strdup("Нет сообщений");
//            chats_info[i]->sender_name = vendor.helpers.strdup("");
        }

        // Отладочный вывод для проверки
        printf("[DEBUG] Chat #%d\n", i);
        printf("  ID: %d\n", chats_info[i]->id);
        printf("  Type: %d\n", chats_info[i]->type);
        printf("  Timestamp: %ld\n", chats_info[i]->timestamp);
        printf("  Name: %s\n", chats_info[i]->name);
//        printf("  Last Message: %s\n", chats_info[i]->last_message->message_text ? chats_info[i]->last_message->message_text : "");
//        printf("  Sender Name: %s\n", chats_info[i]->sender_name);
    }

    chats_info[rows] = NULL;

    qsort(chats_info, rows, sizeof(t_chat_info *), compare_chats);

    // Освобождаем память
    sqlite3_free_table(results);

    return chats_info;
}

void free_chats_info(t_chat_info **chats_info) {
    if (chats_info != NULL) {
        for (int i = 0; chats_info[i] != NULL; i++) {
            free(chats_info[i]);
        }
        free(chats_info);
    }
}

char *format_timestamp(struct tm timestamp) {
    // Получаем текущее время
    time_t now = time(NULL);
    struct tm *now_tm = localtime(&now);

    // Преобразуем `now` в секунды с начала эпохи
    time_t now_seconds = mktime(now_tm);
    time_t timestamp_seconds = mktime(&timestamp);

    // Вычисляем разницу во времени
    double difference = difftime(now_seconds, timestamp_seconds);

    // Выделяем память для строки
    char *buffer = malloc(50);
    if (!buffer) return NULL;

    // Форматируем время в зависимости от разницы
    if (difference < 24 * 60 * 60) {
        // Если меньше суток — формат "HH:MM"
        strftime(buffer, 50, "%H:%M", &timestamp);
    } else if (difference < 7 * 24 * 60 * 60) {
        // Если меньше недели — день недели (например, "Fri")
        strftime(buffer, 50, "%a", &timestamp);
    } else {
        // Если больше недели — формат "day/month/year"
        strftime(buffer, 50, "%d/%m/%Y", &timestamp);
    }

    return buffer;
}



int compare_chats(const void *a, const void *b) {
    t_chat_info *chat_a = *(t_chat_info **)a;
    t_chat_info *chat_b = *(t_chat_info **)b;

    time_t time_a = chat_a->last_message ? mktime(&chat_a->last_message->timestamp) : chat_a->timestamp;
    time_t time_b = chat_b->last_message ? mktime(&chat_b->last_message->timestamp) : chat_b->timestamp;

    if (time_a > time_b) return -1; // Сначала новые
    if (time_a < time_b) return 1;  // Затем старые
    return 0; // Равные
}
