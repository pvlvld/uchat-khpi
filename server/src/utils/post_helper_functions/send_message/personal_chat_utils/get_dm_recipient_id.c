#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>

int get_dm_recipient_id(PGconn *conn, int chat_id, int sender_id) {
    const char *query = "SELECT CASE WHEN user1_id = $1 THEN user2_id ELSE user1_id END AS recipient_id "
                        "FROM personal_chats WHERE chat_id = $2 AND (user1_id = $1 OR user2_id = $1)";
    const char *params[2];
    char chat_id_str[12], sender_id_str[12];
    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);
    snprintf(sender_id_str, sizeof(sender_id_str), "%d", sender_id);
    params[0] = sender_id_str;
    params[1] = chat_id_str;

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error retrieving recipient ID: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1; // Indicate an error
    }

    // Check if a row was returned
    if (PQntuples(res) == 0) {
        fprintf(stderr, "No recipient found for chat_id %d with sender_id %d\n", chat_id, sender_id);
        PQclear(res);
        return -1;
    }

    // Extract the recipient_id
    int recipient_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return recipient_id;
}
