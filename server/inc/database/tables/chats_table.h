#ifndef CHATS_TABLE_H
#define CHATS_TABLE_H
#pragma once

#include <libpq-fe.h>
#include <stdbool.h>

bool delete_chat(PGconn *conn, int chat_id);

char get_chat_type(PGconn *conn, int chat_id);

PGresult *get_user_chats_private(PGconn *conn, int user_id);

PGresult *get_user_chats_group(PGconn *conn, int user_id);
typedef struct {
    bool (*delete_chat)(PGconn *conn, int chat_id);
    char (*get_chat_type)(PGconn *conn, int chat_id);
    PGresult *(*get_user_chats_private)(PGconn *conn, int user_id);
    PGresult *(*get_user_chats_group)(PGconn *conn, int user_id);
} t_chats_table;

#endif // CHATS_TABLE_H
