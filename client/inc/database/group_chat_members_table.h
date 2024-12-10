#ifndef GROUP_CHAT_MEMBERS_TABLE_H
#define GROUP_CHAT_MEMBERS_TABLE_H

#include "database.h"

typedef enum {
    MEMBER,
    ADMIN,
    OWNER,
    LEFT,
    BANNED,
    RESTRICTED,
} t_group_chat_members_rols;

typedef struct {
    t_chats_struct *chat_struct;
    t_users_struct *user_struct;
    t_group_chat_members_rols role;
} t_group_chat_members_struct;

typedef struct {
    void (*create_table)(void);
    void (*add_member)(int chat_id, int user_id, const char *role);
} t_group_chat_members_table;
t_group_chat_members_table init_group_chat_members_table(void);

#endif //GROUP_CHAT_MEMBERS_TABLE_H
