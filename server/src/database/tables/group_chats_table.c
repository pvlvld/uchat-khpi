#include "../../../inc/utils.h"
#include <libpq-fe.h>

int create_group_chat(PGconn *conn, const char *group_name, int group_picture, int background) {
    const char *query = "WITH new_chat AS (INSERT INTO chats (chat_type) VALUES ('group') RETURNING chat_id) "
                        "INSERT INTO group_chats (chat_id, group_name, group_picture, backgroung) "
                        "SELECT chat_id, $1, $2, $3 FROM new_chat "
                        "RETURNING chat_id";
    const char *params[3] = {group_name, itoa(group_picture, NULL), itoa(background, NULL)};

    PGresult *res = PQexecParams(conn, query, 3, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Create group chat failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    int chat_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);
    return chat_id;
}

bool delete_group_chat(PGconn *conn, int chat_id) {
    const char *query = "DELETE FROM chats WHERE chat_id = $1";
    char chat_id_str[12];
    const char *params[1] = {itoa(chat_id, chat_id_str)};

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Delete group chat failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    return false;
}

bool update_group_chat_name(PGconn *conn, int chat_id, const char *group_name) {
    const char *query = "UPDATE group_chats SET group_name = $1, updated_ad = NOW() WHERE chat_id = $2";
    char chat_id_str[12];
    const char *params[2] = {group_name, itoa(chat_id, chat_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Update group chat name failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

bool delete_group_chat_picture(PGconn *conn, int chat_id) {
    const char *query = "UPDATE group_chats SET group_picture = NULL, updated_ad = NOW() WHERE chat_id = $1";
    char chat_id_str[12];
    const char *params[1] = {itoa(chat_id, chat_id_str)};

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Delete group chat picture failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

bool update_group_chat_picture(PGconn *conn, int chat_id, int media_id) {
    const char *query = "UPDATE group_chats SET group_picture = $1, updated_ad = NOW() WHERE chat_id = $2";
    char chat_id_str[12], media_id_str[12];
    const char *params[2] = {itoa(media_id, media_id_str), itoa(chat_id, chat_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Update group chat picture failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

bool update_group_chat_about(PGconn *conn, int chat_id, const char *about) {
    const char *query = "UPDATE group_chats SET about = $1, updated_ad = NOW() WHERE chat_id = $2";
    char chat_id_str[12];
    const char *params[2] = {about, itoa(chat_id, chat_id_str)};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Update group chat about failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}
