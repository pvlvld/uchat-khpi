#ifndef POSTGRES_H
#define POSTGRES_H

#pragma once

#include "../../libraries/cJSON/cJSON.h"
#include "database/database_pool.h"
#include "database/tables/messages_table.h"
#include "database/tables/personal_chat_table.h"
#include "database/tables/users_table.h"

typedef struct {
    t_database_pool pool;
    t_messages_table messages_table;
    t_personal_chat_table personal_chat_table;
    t_users_table users_table;
} t_database;

void init_database(t_database *database);

#endif // POSTGRES_H
