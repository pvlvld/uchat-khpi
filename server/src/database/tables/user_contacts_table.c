#include "../../../inc/utils.h"
#include <libpq-fe.h>

bool send_friend_request(PGconn *conn, int user_id, int contact_id) {
    const char *query = "INSERT INTO user_contacts (user_id, contact_id) VALUES ($1, $2)";
    char user_id_str[12], contact_id_str[12];
    const char *params[2] = {itoa(user_id, user_id_str), itoa(contact_id, contact_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Send friend request failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool accept_friend_request(PGconn *conn, int user_id, int contact_id) {
    const char *query = "UPDATE user_contacts SET status = 'accepted' WHERE user_id = $1 AND contact_id = $2";
    char user_id_str[12], contact_id_str[12];
    const char *params[2] = {itoa(user_id, user_id_str), itoa(contact_id, contact_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Accept friend request failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool reject_friend_request(PGconn *conn, int user_id, int contact_id) {
    const char *query = "UPDATE user_contacts SET status = 'rejected' WHERE user_id = $1 AND contact_id = $2";
    char user_id_str[12], contact_id_str[12];
    const char *params[2] = {itoa(user_id, user_id_str), itoa(contact_id, contact_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Reject friend request failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool delete_friend(PGconn *conn, int user_id, int contact_id) {
    const char *query = "DELETE FROM user_contacts WHERE user_id = $1 AND contact_id = $2";
    char user_id_str[12], contact_id_str[12];
    const char *params[2] = {itoa(user_id, user_id_str), itoa(contact_id, contact_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Delete friend failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool is_friend(PGconn *conn, int user_id, int contact_id) {
    const char *query = "SELECT 1 FROM user_contacts WHERE user_id = $1 AND contact_id = $2 AND status = 'accepted'";
    char user_id_str[12], contact_id_str[12];
    const char *params[2] = {itoa(user_id, user_id_str), itoa(contact_id, contact_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) == 1) {
        PQclear(res);
        return true;
    }

    PQclear(res);
    return false;
}
