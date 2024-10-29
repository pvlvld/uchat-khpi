#include "../../../inc/utils.h"
#include <libpq-fe.h>
#include <stdbool.h>
#include <stdlib.h>

int create_personal_chat(PGconn *conn, int user1_id, int user2_id) {
    const char *query = "INSERT INTO personal_chats (chat_id, user1_id, user2_id) "
                        "VALUES (DEFAULT, LEAST($1::int, $2::int), GREATEST($1::int, $2::int)) "
                        "RETURNING chat_id";

    char user1_id_str[12], user2_id_str[12];
    const char *params[2] = {itoa(user1_id, user1_id_str), itoa(user2_id, user2_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    int chat_id = -1;
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Create personal chat failed: %s", PQerrorMessage(conn));
    } else {
        chat_id = atoi(PQgetvalue(res, 0, 0));
    }

    PQclear(res);
    return chat_id;
}

PGresult *get_personal_chat(PGconn *conn, int user1_id, int user2_id) {
    const char *query = "SELECT * FROM personal_chats "
                        "WHERE user1_id = LEAST($1::int, $2::int) "
                        "AND user2_id = GREATEST($1::int, $2::int)";

    char user1_id_str[12], user2_id_str[12];
    const char *params[2] = {itoa(user1_id, user1_id_str), itoa(user2_id, user2_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Get personal chat failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    return res; // Caller is responsible for freeing with PQclear.
}

bool delete_personal_chat(PGconn *conn, int user1_id, int user2_id) {
    const char *query = "DELETE FROM personal_chats "
                        "WHERE user1_id = LEAST($1::int, $2::int) "
                        "AND user2_id = GREATEST($1::int, $2::int)";

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

bool update_background_personal_chat(PGconn *conn, int user1_id, int user2_id, int media_id) {
    const char *query = "UPDATE personal_chats "
                        "SET background = $1 "
                        "WHERE user1_id = LEAST($2::int, $3::int) "
                        "AND user2_id = GREATEST($2::int, $3::int)";

    char media_id_str[12], user1_id_str[12], user2_id_str[12];
    const char *params[3] = {itoa(media_id, media_id_str), itoa(user1_id, user1_id_str), itoa(user2_id, user2_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Updating personal chat background failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

PGresult *list_user_personal_chats(PGconn *conn, int user_id) {
    const char *query = "SELECT * FROM personal_chats WHERE user1_id = $1 OR user2_id = $1";

    char user_id_str[12];
    const char *params[1] = {itoa(user_id, user_id_str)};

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "List user personal chats failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    return res; // Caller is responsible for freeing with PQclear.
}
