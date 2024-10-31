#ifndef PERSONAL_CHATS_TABLE_H
#define PERSONAL_CHATS_TABLE_H
#pragma once

#include <libpq-fe.h>

int create_personal_chat(PGconn *conn, int user1_id, int user2_id);

PGresult *get_personal_chat(PGconn *conn, int user1_id, int user2_id);

bool delete_personal_chat(PGconn *conn, int user1_id, int user2_id);

bool update_background_personal_chat(PGconn *conn, int user1_id, int user2_id, int media_id);

PGresult *list_user_personal_chats(PGconn *conn, int user_id);

typedef struct {
    int (*create_personal_chat)(PGconn *conn, int user1_id, int user2_id);
    PGresult *(*get_personal_chat)(PGconn *conn, int user1_id, int user2_id);
    bool (*delete_personal_chat)(PGconn *conn, int user1_id, int user2_id);
    bool (*update_background_personal_chat)(PGconn *conn, int user1_id, int user2_id, int media_id);
    PGresult *(*list_user_personal_chats)(PGconn *conn, int user_id);
} t_personal_chat_table;

#endif // PERSONAL_CHATS_TABLE_H
