#include "../../../inc/utils.h"
#include <libpq-fe.h>
#include <stdbool.h>
#include <stdlib.h>

int create_user(PGconn *conn, const char *username, const char *user_login, const char *password_hash,
                const char *public_key, const char *locale) {
    const char *query = "INSERT INTO users (username, user_login, password_hash, public_key, locale) VALUES ($1, $2, "
                        "$3, $4, $5) RETURNING user_id";
    const char *params[5] = {username, user_login, password_hash, public_key, locale};
    PGresult *res = PQexecParams(conn, query, 5, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int user_id = atoi(PQgetvalue(res, 0, 0));
        PQclear(res);
        return user_id;
    }

    fprintf(stderr, "Create user failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return -1;
}

PGresult *get_user_by_login(PGconn *conn, const char *user_login) {
    const char *query = "SELECT * FROM users WHERE user_login = $1";
    const char *params[1] = {user_login};
    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Get user failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    // wth? -vld
    if (PQntuples(res) == 0) {
        PQclear(res);
        return NULL;
    }

    return res; // Caller is responsible for freeing with PQclear.
}

PGresult *get_user_by_id(PGconn *conn, int user_id) {
    const char *query = "SELECT * FROM users WHERE user_id = $1";
    char user_id_str[12];
    const char *params[1] = {itoa(user_id, user_id_str)};
    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Get user failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    return res; // Caller is responsible for freeing with PQclear.
}

PGresult *get_user_by_username(PGconn *conn, const char *username) {
    const char *query = "SELECT * FROM users WHERE username = $1";
    const char *params[1] = {username};
    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Get user by username failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return NULL;
    }

    return res; // Caller is responsible for freeing with PQclear.
}

bool update_user_locale(PGconn *conn, int user_id, const char *locale) {
    const char *query = "UPDATE users SET locale = $1, updated_at = NOW() WHERE user_id = $2";
    char user_id_str[12];
    const char *params[2] = {locale, itoa(user_id, user_id_str)};
    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Update user locale failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool update_user_username(PGconn *conn, int user_id, const char *username) {
    const char *query = "UPDATE users SET username = $1, updated_at = NOW() WHERE user_id = $2";
    char user_id_str[12];
    const char *params[2] = {username, itoa(user_id, user_id_str)};
    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Update user username failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool delete_user(PGconn *conn, int user_id) {
    const char *query = "UPDATE users SET deleted_at = NOW(), updated_at = NOW() WHERE user_id = $1";
    char user_id_str[12];
    const char *params[1] = {itoa(user_id, user_id_str)};
    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Delete user failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool restore_user(PGconn *conn, int user_id) {
    const char *query = "UPDATE users SET deleted_at = NULL, updated_at = NOW() WHERE user_id = $1";
    char user_id_str[12];
    const char *params[1] = {itoa(user_id, user_id_str)};
    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Restore user failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool update_user_password(PGconn *conn, int user_id, const char *password_hash) {
    const char *query = "UPDATE users SET password_hash = $1, updated_at = NOW() WHERE user_id = $2";
    char user_id_str[12];
    const char *params[2] = {password_hash, itoa(user_id, user_id_str)};
    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Update user password failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool update_user_about(PGconn *conn, int user_id, const char *about) {
    const char *query = "UPDATE users SET about = $1, updated_at = NOW() WHERE user_id = $2";
    char user_id_str[12];
    const char *params[2] = {about, itoa(user_id, user_id_str)};
    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Update user about failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool update_user_profile_picture(PGconn *conn, int user_id, int profile_picture_id) {
    const char *query = "UPDATE users SET profile_picture = $1, updated_at = NOW() WHERE user_id = $2";
    char user_id_str[12], profile_picture_str[12];
    const char *params[2] = {itoa(profile_picture_id, profile_picture_str), itoa(user_id, user_id_str)};
    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Update user profile picture failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool delete_user_profile_picture(PGconn *conn, int user_id) {
    const char *query = "UPDATE users SET profile_picture = NULL, updated_at = NOW() WHERE user_id = $1";
    char user_id_str[12];
    const char *params[1] = {itoa(user_id, user_id_str)};
    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Delete user profile picture failed: %s", PQerrorMessage(conn));
    PQclear(res);
    return false;
}