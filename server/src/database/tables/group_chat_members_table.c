#include "../../../inc/utils.h"
#include <libpq-fe.h>

PGresult *get_user_chats_group(PGconn *conn, int user_id) {
    const char *query = "SELECT chat_id FROM group_chat_members WHERE user_id = $1";
    char user_id_str[12];
    const char *params[1] = {itoa(user_id, user_id_str)};

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Get user chats failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    return res; // Caller is responsible for freeing with PQclear.
}

bool add_chat_member(PGconn *conn, int chat_id, int user_id, char *role) {
    const char *query = "INSERT INTO group_chat_members (chat_id, user_id, role) VALUES ($1, $2, $3)";
    char chat_id_str[12], user_id_str[12];
    const char *params[3] = {itoa(chat_id, chat_id_str), itoa(user_id, user_id_str), role};

    PGresult *res = PQexecParams(conn, query, 3, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Add chat member failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

bool remove_chat_member(PGconn *conn, int chat_id, int user_id) {
    const char *query = "DELETE FROM group_chat_members WHERE chat_id = $1 AND user_id = $2";
    char chat_id_str[12], user_id_str[12];
    const char *params[2] = {itoa(chat_id, chat_id_str), itoa(user_id, user_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Remove chat member failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}







