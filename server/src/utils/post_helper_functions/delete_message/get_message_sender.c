#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "../../../../inc/utils.h"

MessageSenderResult_t get_message_sender(PGconn *conn, int chat_id, int message_id) {
    MessageSenderResult_t result = { .Success = 0, .sender_id = -1 };

    const char *query =
        "SELECT sender_id "
        "FROM messages "
        "WHERE chat_id = $1 AND message_id = $2";

    char chat_id_str[12], message_id_str[12];
    const char *params[2] = {
        itoa(chat_id, chat_id_str),
        itoa(message_id, message_id_str)
    };

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    // Check for errors
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error: Failed to retrieve message sender: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return result;
    }

    // Check if the query returned a result
    if (PQntuples(res) == 0) {
        fprintf(stderr, "No sender found for chat_id=%d and message_id=%d\n", chat_id, message_id);
        PQclear(res);
        return result;
    }

    // Extract sender_id
    result.sender_id = atoi(PQgetvalue(res, 0, 0));
    result.Success = 1;

    PQclear(res);
    return result;
}
