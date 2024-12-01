#include "../../../inc/utils.h"

int create_personal_chat(PGconn *conn, int user1_id, int user2_id) {
    const char *query = "WITH new_chat AS (INSERT INTO chats (chat_type) VALUES ('personal') RETURNING chat_id) "
                        "INSERT INTO personal_chats (chat_id, user1_id, user2_id) "
                        "SELECT chat_id, $2, $3 FROM new_chat "
                        "RETURNING chat_id";

    char user1_id_str[12], user2_id_str[12];
    const char *params[2] = {itoa(user1_id, user1_id_str), itoa(user2_id, user2_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Create personal chat failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    int chat_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return chat_id;
}

bool delete_personal_chat(PGconn *conn, int user1_id, int user2_id) {
    const char *query =
        "DELETE FROM chats WHERE chat_id IN (SELECT chat_id FROM personal_chats WHERE user1_id = $1 AND user2_id = $2)";
    char user1_id_str[12], user2_id_str[12];
    const char *params[2] = {itoa(user1_id, user1_id_str), itoa(user2_id, user2_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Delete personal chat failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    return false;
}
