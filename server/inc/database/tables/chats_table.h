#ifndef CHATS_TABLE_H
#define CHATS_TABLE_H
#pragma once

#include <libpq-fe.h>
#include <stdbool.h>

bool delete_chat(PGconn *conn, int chat_id);

typedef struct {
    bool (*delete_chat)(PGconn *conn, int chat_id);
} t_chats_table;

#endif // CHATS_TABLE_H
