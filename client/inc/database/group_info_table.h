#ifndef GROUP_INFO_TABLE_H
#define GROUP_INFO_TABLE_H

#include "database.h"

// Структура для хранения информации о группе
typedef struct {
    t_chats_struct *chat_struct;
    char *group_name;
    int group_picture;
} t_group_info_struct;

// Структура таблицы group_info
typedef struct {
    void (*create_table)(void);
} t_group_info_table;

// Функция инициализации таблицы group_info
t_group_info_table init_group_info_table(void);

#endif //GROUP_INFO_TABLE_H
