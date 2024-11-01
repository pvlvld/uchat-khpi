#include "../../../inc/utils.h"
#include <libpq-fe.h>
#include <stdbool.h>

bool send_message(PGconn *conn, int chat_id, int sender_id, const char *message_text, int media_id, int reply_to_chat,
                  int reply_to_message, int forwarded_from_chat, int forwarded_from_message) {
    const char *query =
        "INSERT INTO messages (chat_id, sender_id, message_text, media, reply_to_chat, reply_to_message, "
        "forwarded_from_chat, forwarded_from_message) VALUES ($1, $2, $3, $4, $5, $6, $7, $8)";
    char chat_id_str[12], sender_id_str[12], media_id_str[12], reply_to_chat_str[12], reply_to_message_str[12],
        forwarded_from_chat_str[12], forwarded_from_message_str[12];
    const char *params[8] = {itoa(chat_id, chat_id_str),
                             itoa(sender_id, sender_id_str),
                             message_text,
                             media_id ? itoa(media_id, media_id_str) : NULL,
                             reply_to_chat ? itoa(reply_to_chat, reply_to_chat_str) : NULL,
                             reply_to_message ? itoa(reply_to_message, reply_to_message_str) : NULL,
                             forwarded_from_chat ? itoa(forwarded_from_chat, forwarded_from_chat_str) : NULL,
                             forwarded_from_message ? itoa(forwarded_from_message, forwarded_from_message_str) : NULL};

    PGresult *res = PQexecParams(conn, query, 8, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Send message failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

bool delete_message(PGconn *conn, int chat_id, int message_id) {
    const char *query = "UPDATE messages SET deleted = true WHERE chat_id = $1 AND message_id = $2";
    char chat_id_str[12], message_id_str[12];
    const char *params[2] = {itoa(chat_id, chat_id_str), itoa(message_id, message_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Delete message failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

bool edit_message_text(PGconn *conn, int chat_id, int message_id, const char *new_message_text) {
    const char *query = "UPDATE messages SET message_text = $1, edited = true WHERE chat_id = $2 AND message_id = $3";
    char chat_id_str[12], message_id_str[12];
    const char *params[3] = {new_message_text, itoa(chat_id, chat_id_str), itoa(message_id, message_id_str)};

    PGresult *res = PQexecParams(conn, query, 3, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Edit message failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

PGresult *get_message(PGconn *conn, int chat_id, int message_id) {
    const char *query = "SELECT * FROM messages WHERE chat_id = $1 AND message_id = $2";
    char chat_id_str[12];
    char message_id_str[12];
    const char *params[2] = {itoa(chat_id, chat_id_str), itoa(message_id, message_id_str)};
    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Get message failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    return res; // Caller is responsible for freeing with PQclear.
}

PGresult *get_messages(PGconn *conn, int chat_id, int limit, int offset) {
    const char *query = "SELECT * FROM messages WHERE chat_id = $1 ORDER BY created_at DESC LIMIT $2 OFFSET $3";
    char chat_id_str[12], limit_str[12], offset_str[12];
    const char *params[3] = {itoa(chat_id, chat_id_str), itoa(limit, limit_str), itoa(offset, offset_str)};
    PGresult *res = PQexecParams(conn, query, 3, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Get messages failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    return res; // Caller is responsible for freeing with PQclear.
}
