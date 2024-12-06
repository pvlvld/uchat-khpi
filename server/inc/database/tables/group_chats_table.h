#ifndef GROUP_CHATS_TABLE_H
#define GROUP_CHATS_TABLE_H
#pragma once

#include <libpq-fe.h>
#include <stdbool.h>

int create_group_chat(PGconn *conn, const char *group_name, int group_picture, int background);

bool delete_group_chat(PGconn *conn, int chat_id);

bool update_group_chat_name(PGconn *conn, int chat_id, const char *group_name);

bool delete_group_chat_picture(PGconn *conn, int chat_id);

bool update_group_chat_picture(PGconn *conn, int chat_id, int media_id);

bool update_group_chat_about(PGconn *conn, int chat_id, const char *about);

typedef struct {
    int (*create_group_chat)(PGconn *conn, const char *group_name, int group_picture, int background);
    bool (*delete_group_chat)(PGconn *conn, int chat_id);
    bool (*update_group_chat_name)(PGconn *conn, int chat_id, const char *group_name);
    bool (*delete_group_chat_picture)(PGconn *conn, int chat_id);
    bool (*update_group_chat_picture)(PGconn *conn, int chat_id, int media_id);
    bool (*update_group_chat_about)(PGconn *conn, int chat_id, const char *about);
} t_group_chats_table;

#endif // GROUP_CHATS_TABLE_H
