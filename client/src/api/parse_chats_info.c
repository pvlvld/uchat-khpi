#include "../../inc/header.h"

char *strdup(const char *str) {
    size_t len = strlen(str) + 1;
    char *copy = malloc(len);
    if (copy) {
        memcpy(copy, str, len);
    }
    return copy;
}

t_chat_info **parse_chats_info(void) {
    // SQL-запрос для получения всех чатов
    const char *sql = "SELECT chat_id, chat_type, created_at FROM chats ORDER BY created_at DESC;";
    char **results = NULL;
    int rows, cols;
    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    // Проверка результата выполнения SQL-запроса
    if (rc != 0) {
        printf("[ERROR] Ошибка при выполнении SQL-запроса: %s\n", sqlite3_errmsg(vendor.database.db));
        return NULL;
    }

    if (rows == 0) {
        printf("[INFO] Чаты не найдены. Таблица 'chats' пустая.\n");
        return NULL;
    }

    printf("[DEBUG] Найдено строк: %d, Колонок: %d\n", rows, cols);

    // Выделяем память для массива указателей на структуры t_chat_info
    t_chat_info **chats_info = malloc(sizeof(t_chat_info *) * (rows + 1));
    if (!chats_info) {
        printf("[ERROR] Ошибка выделения памяти для массива структур.\n");
        return NULL;
    }

    // Заполняем массив структур
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

        // Получаем значения из результата запроса
        chats_info[i]->id = atoi(results[(i + 1) * cols]);
        const char *type_str = results[(i + 1) * cols + 1];
        chats_info[i]->type = (strcmp(type_str, "personal") == 0) ? PERSONAL :
                              (strcmp(type_str, "group") == 0) ? GROUP : CHANNEL;
        chats_info[i]->timestamp = (time_t)atoll(results[(i + 1) * cols + 2]);

        // Задаем дополнительные поля
        if (chats_info[i]->type == PERSONAL) {
            // Получаем имя собеседника в персональном чате
            int other_user_id = get_other_user_id(chats_info[i]->id);
            chats_info[i]->name = get_user_name(other_user_id);
        } else if (chats_info[i]->type == GROUP) {
            // Получаем имя группы в групповом чате
            chats_info[i]->name = get_group_name_by_chat_id(chats_info[i]->id);
        } else {
            chats_info[i]->name = strdup("Неизвестный чат");
        }

        chats_info[i]->path_to_logo = "default_logo.jpg";  // Путь к логотипу (по умолчанию)
        chats_info[i]->last_message = strdup("Последнее сообщение");
        chats_info[i]->sender_name = strdup("Отправитель");
        chats_info[i]->unreaded_messages = 0;

        // Отладочный вывод для проверки заполнения структуры
        printf("[DEBUG] Chat #%d\n", i);
        printf("  ID: %d\n", chats_info[i]->id);
        printf("  Type: %d\n", chats_info[i]->type);
        printf("  Timestamp: %ld\n", chats_info[i]->timestamp);
        printf("  Name: %s\n", chats_info[i]->name);
        printf("  Last Message: %s\n", chats_info[i]->last_message);
        printf("  Sender Name: %s\n", chats_info[i]->sender_name);
    }

    chats_info[rows] = NULL;

    // Освобождаем память, выделенную для результатов SQL-запроса
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

char *format_timestamp(time_t timestamp) {
    time_t now = time(NULL);
    double difference = difftime(now, timestamp); // Разница во времени

    // Преобразуем timestamp из миллисекунд в секунды
    timestamp /= 1000;

    char *buffer = malloc(50); // Выделяем память для строки
    if (buffer == NULL) {
        return NULL; // Возвращаем NULL в случае ошибки выделения памяти
    }

    // Создаем структуру tm для локального времени
    struct tm *tm = localtime(&timestamp);

    if (difference < 24 * 60 * 60) {
        // Форматируем время в формате "HH:MM"
        strftime(buffer, 50, "%H:%M", tm);
    }

    // 1. Если разница меньше недели
    else if (difference < 7 * 24 * 60 * 60) {
        // Форматируем день недели (например, "Fri")
        strftime(buffer, 50, "%a", tm); // %a возвращает краткое название дня недели
    }
    // 2. Если разница больше недели
    else {
        // Форматируем дату в формате "day/month/year"
        strftime(buffer, 50, "%d/%m/%Y", tm); // Формат "день/месяц/год"
    }

    return buffer; // Возвращаем указатель на строку
}

int compare_chats(const void *a, const void *b) {
    t_chat_info *chatA = *(t_chat_info **)a;
    t_chat_info *chatB = *(t_chat_info **)b;

    if (chatA->timestamp > chatB->timestamp) return 1;
    if (chatA->timestamp < chatB->timestamp) return -1;
    return 0;
}
