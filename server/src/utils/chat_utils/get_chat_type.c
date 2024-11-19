#include "../../../inc/utils.h"
#include <libpq-fe.h>
#include <stdlib.h>
#include <string.h>

const char *get_chat_type(PGconn *conn, int chat_id) {
    const char *query = "SELECT chat_type FROM chats WHERE chat_id = $1";
    const char *params[1];
    char chat_id_str[12];
    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);
    params[0] = chat_id_str;

    // Execute the query
    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error retrieving chat type: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    if (PQntuples(res) == 0) {
        fprintf(stderr, "No chat found with chat_id %d\n", chat_id);
        PQclear(res);
        return NULL;
    }

    const char *chat_type = PQgetvalue(res, 0, 0);
    char *chat_type_copy = mx_strdup(chat_type);

    PQclear(res);
    return chat_type_copy; // Caller is responsible for freeing this memory
}
