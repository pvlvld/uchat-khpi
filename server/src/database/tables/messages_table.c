#include <libpq-fe.h>
#include <stdbool.h>

bool send_message(PGconn *conn, int chat_id, int sender_id, const char *message_text, int media_id, int reply_to_chat,
                  int reply_to_message, int forwarded_from_chat, int forwarded_from_message) {
    const char *query =
        "INSERT INTO messages (chat_id, sender_id, message_text, media, reply_to_chat, reply_to_message, "
        "forwarded_from_chat, forwarded_from_message) VALUES ($1, $2, $3, $4, $5, $6, $7, $8)";
    const char *params[8];
    bool status = false;
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
    } else {
        // printf("Message sent successfully.\n");
        status = true;
    }

    PQclear(res);
    return status;
}
