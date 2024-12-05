#include "../../../inc/utils.h"

bool delete_chat(PGconn *conn, int chat_id) {
    const char *query = "DELETE FROM chats WHERE chat_id = $1";
    char chat_id_str[12];
    const char *params[1] = {itoa(chat_id, chat_id_str)};

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Delete chat failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

char get_chat_type(PGconn *conn, int chat_id) {
    const char *query = "SELECT chat_type FROM chats WHERE chat_id = $1";
    char chat_id_str[12];
    const char *params[1] = {itoa(chat_id, chat_id_str)};

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Get chat type failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return '\0';
    }

    char chat_type = *PQgetvalue(res, 0, 0);
    PQclear(res);
    return chat_type;
}

PGresult *get_user_chats_private(PGconn *conn, int user_id) {
    const char *query = "SELECT chat_id FROM personal_chats WHERE user1_id = $1 OR user2_id = $1";
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
