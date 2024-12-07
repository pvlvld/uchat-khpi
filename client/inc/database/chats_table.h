#ifndef CHATS_TABLE_H
#define CHATS_TABLE_H

#include "database.h"

typedef enum {
    PERSONAL,
    GROUP,
    CHANNEL,
} t_chats_types;

typedef struct {
    int chat_id;
    t_chats_types chat_type;
    struct tm created_at;
} t_chats_struct;

typedef struct {
    void (*create_table)(void);
    void (*add_chat)(int chat_id, const char *chat_type);
    t_chats_struct *(*get_chat_by_id)(int chat_id);
    void (*free_struct)(t_chats_struct *chat);
    void (*delete_chat_and_related_data)(int chat_id, t_chats_types group_type);
} t_chats_table;
t_chats_table init_chats_table(void);

#endif //CHATS_TABLE_H
