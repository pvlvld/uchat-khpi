#include "../../../inc/utils.h"
#include <libpq-fe.h>
#include <stdbool.h>

bool send_message(PGconn *conn, int chat_id, int sender_id, const char *message_text, int media_id, int reply_to_chat,
                  int reply_to_message, int forwarded_from_chat, int forwarded_from_message) {
    const char *query =
        "INSERT INTO messages (chat_id, sender_id, message_text, media, reply_to_chat, reply_to_message, "
        "forwarded_from_chat, forwarded_from_message) VALUES ($1, $2, $3, $4, $5, $6, $7, $8)";
    const char *params[8];
    char chat_id_str[12], sender_id_str[12], media_id_str[12], reply_to_chat_str[12], reply_to_message_str[12],
        forwarded_from_chat_str[12], forwarded_from_message_str[12];

    snprintf(chat_id_str, 12, "%d", chat_id);
    snprintf(sender_id_str, 12, "%d", sender_id);
    snprintf(media_id_str, 12, "%d", media_id);
    snprintf(reply_to_chat_str, 12, "%d", reply_to_chat);
    snprintf(reply_to_message_str, 12, "%d", reply_to_message);
    snprintf(forwarded_from_chat_str, 12, "%d", forwarded_from_chat);
    snprintf(forwarded_from_message_str, 12, "%d", forwarded_from_message);

    params[0] = chat_id_str;
    params[1] = sender_id_str;
    params[2] = message_text;
    params[3] = media_id ? media_id_str : NULL;
    params[4] = reply_to_chat ? reply_to_chat_str : NULL;
    params[5] = reply_to_message ? reply_to_message_str : NULL;
    params[6] = forwarded_from_chat ? forwarded_from_chat_str : NULL;
    params[7] = forwarded_from_message ? forwarded_from_message_str : NULL;

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

bool edit_message(PGconn *conn, int chat_id, int message_id, const char *new_message_text) {
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