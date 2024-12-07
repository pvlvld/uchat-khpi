#ifndef PERSONAL_CHATS_TABLE_H
#define PERSONAL_CHATS_TABLE_H

#include "database.h"

typedef struct {
    t_chats_struct *chat_struct;
    t_chats_struct *user1_struct;
    t_chats_struct *user2_struct;
} t_personal_chats_struct;

typedef struct {
    void (*create_table)(void);
    t_users_struct **(*get_users)(int *user_count, int chat_id);
    int (*create_personal_chat)(int chat_id, int user2_id);
} t_personal_chats_table;
t_personal_chats_table init_personal_chats_table(void);

#endif //PERSONAL_CHATS_TABLE_H
