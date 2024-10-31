#ifndef MESSAGES_TABLE_H
#define MESSAGES_TABLE_H
#pragma once

#include <libpq-fe.h>

bool send_message(PGconn *conn, int chat_id, int sender_id, const char *message_text, int media_id, int reply_to_chat,
                  int reply_to_message, int forwarded_from_chat, int forwarded_from_message);

bool delete_message(PGconn *conn, int chat_id, int message_id);

bool edit_message_text(PGconn *conn, int chat_id, int message_id, const char *new_message_text);

typedef struct {
    bool (*send_message)(PGconn *conn, int chat_id, int sender_id, const char *message_text, int media_id,
                         int reply_to_chat, int reply_to_message, int forwarded_from_chat, int forwarded_from_message);
    bool (*delete_message)(PGconn *conn, int chat_id, int message_id);
    bool (*edit_message_text)(PGconn *conn, int chat_id, int message_id, const char *new_message_text);
} t_messages_table;

#endif // MESSAGES_TABLE_H
