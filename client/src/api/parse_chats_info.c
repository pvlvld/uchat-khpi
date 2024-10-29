#include "../../inc/header.h"

t_chat_info **parse_chats_info(void) {
    // Массив указателей на структуры t_chat_info
    t_chat_info **chats_info = malloc(sizeof(t_chat_info *) * 13);

    int _id[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    char *_name[12] = {"Chat name 1", "Chat name 2", "Chat name 3",
                       "Chat name 4", "Chat name 5", "Chat name 6",
                       "Chat name 7", "Chat name 8", "Chat name 9",
                       "Chat name 10", "Chat name 11", "Chat name 12"};
    char *_last_message[2] = {
        "Інформація стосовно того, хто вдало подолав марафон вже майже готова, залишилося зовсім трошки Поки ми фіналізуєио питання, нагадуємо, що свій фідбек стосовно марафону ти можеш залишити у цій формі",
        "The information on who successfully completed the marathon is almost ready, just a little bit left "
    };
    int _type[12] = {0, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0};
    char *_path_to_logo[4] = {"person_img.jpg", "logo_2.jpg", "logo_3.jpg", "logo_4.jpg"};
    char *_sender_name[6] = {"Username1", "Username2", "Username3", "Username4", "Username5", "Username6"};
    int _unreaded_messages[5] = {0, 20, 0, 10, 100};


    for (int i = 0; i < 12; i++) {
        chats_info[i] = malloc(sizeof(t_chat_info));
        if (chats_info[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(chats_info[j]);
            }
            free(chats_info);
            return NULL;
        }

        chats_info[i]->id = _id[i];
        chats_info[i]->type = _type[i];
        chats_info[i]->name = _name[i];
        chats_info[i]->path_to_logo = _path_to_logo[i % 4];
        chats_info[i]->last_message = _last_message[i % 2];
        chats_info[i]->unreaded_messages = _unreaded_messages[rand() % 5];
        chats_info[i]->sender_name = _sender_name[i % 6];

        time_t now = time(NULL);
        int random_seconds = rand() % (8 * 24 * 60 * 60);
        chats_info[i]->timestamp = now - random_seconds;
    }

    chats_info[12] = NULL;

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
