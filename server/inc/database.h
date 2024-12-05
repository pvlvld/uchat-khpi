#ifndef POSTGRES_H
#define POSTGRES_H

#pragma once

#include "../../libraries/cJSON/cJSON.h"
#include "database/database_pool.h"
#include "database/tables/messages_table.h"
#include "database/tables/personal_chat_table.h"
#include "database/tables/users_table.h"
#include "database/tables/media_table.h"
#include "database/tables/chats_table.h"
#include "database/tables/group_chats_table.h"
#include "database/tables/group_chat_members_table.h"

typedef struct {
    t_database_pool pool;
    t_messages_table messages_table;
    t_personal_chat_table personal_chat_table;
    t_users_table users_table;
    t_media_table media_table;
    t_chats_table chat_table;
    t_group_chats_table group_chats_table;
    t_group_chat_members_table group_chat_members_table;
} t_database;

t_database *init_database(void);

#endif // POSTGRES_H
