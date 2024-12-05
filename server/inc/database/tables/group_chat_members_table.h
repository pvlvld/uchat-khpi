#ifndef GROUP_CHAT_MEMBERS_TABLE_H
#define GROUP_CHAT_MEMBERS_TABLE_H
#pragma once

#include <libpq-fe.h>
#include <stdbool.h>


PGresult *get_user_chats_group(PGconn *conn, int user_id);
bool add_chat_member(PGconn *conn, int chat_id, int user_id);
bool remove_chat_member(PGconn *conn, int chat_id, int user_id);

typedef struct {
    PGresult *(*get_user_chats_group)(PGconn *conn, int user_id);
    bool (*add_chat_member)(PGconn *conn, int chat_id, int user_id);
    bool (*remove_chat_member)(PGconn *conn, int chat_id, int user_id);
} t_group_chat_members_table;

#endif // GROUP_CHAT_MEMBERS_TABLE_H
