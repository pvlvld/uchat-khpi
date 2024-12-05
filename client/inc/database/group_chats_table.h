#ifndef GROUP_CHATS_TABLE_H
#define GROUP_CHATS_TABLE_H

#include "database.h"

typedef struct {
    t_chats_struct *chat_struct;
    char *group_name;
} t_group_chats_struct;

typedef struct {
    void (*create_table)(void);
    int (*create_group)(int group_id, const char *group_name);
} t_group_chats_table;
t_group_chats_table init_group_chats_table(void);

#endif //GROUP_CHATS_TABLE_H
