#include "../../inc/utils.h"

const char *get_user_role_in_group(PGconn *conn, int chat_id, int user_id) {
    const char *query = "SELECT role FROM group_chat_members WHERE chat_id = $1 AND user_id = $2";
    const char *params[2];
    char chat_id_str[12], user_id_str[12];

    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);
    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);
    params[0] = chat_id_str;
    params[1] = user_id_str;

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);
    const char *role = NULL;

    if (PQntuples(res) > 0) { role = strdup(PQgetvalue(res, 0, 0)); }

    PQclear(res);
    return role; // Caller is responsible for freeing this memory
}
