#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include "../../../../inc/handlers/get_handlers/get_all_updates_after_offline.h"
/**
 * Function to check if a user's information has been updated since a given timestamp.
 *
 * @param conn - The PostgreSQL connection object.
 * @param user_id - The ID of the user to check.
 * @param timestamp - The timestamp to compare against.
 * @return PGresult* - A PostgreSQL result object containing the user's ID, username, and user_login if updated, or NULL if not updated.
 */

PGresult *is_user_updated(PGconn *conn, int user_id, time_t timestamp) {
    const char *query =
        "SELECT user_id, username, user_login "
        "FROM users "
        "WHERE user_id = $1 AND updated_at > to_timestamp($2)";

    char user_id_str[12];
    char timestamp_str[20];

    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);
    snprintf(timestamp_str, sizeof(timestamp_str), "%ld", timestamp);

    const char *params[2] = {user_id_str, timestamp_str};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error checking user update: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    // If no rows are returned, it means the user has not been updated.
    if (PQntuples(res) == 0) {
        PQclear(res);
        return NULL;
    }

    return res; // Caller is responsible for freeing the result with PQclear.
}

PGresult *get_group_members(PGconn *conn, int chat_id) {
    // Define the SQL query
    const char *query = "SELECT user_id, role FROM group_chat_members WHERE chat_id = $1";

    // Prepare parameters
    char chat_id_str[12];
    const char *params[1] = {itoa(chat_id, chat_id_str)};

    // Execute the query
    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    // Check for execution errors
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Get group members failed: %s\n", PQerrorMessage(conn));
        PQclear(res); // Clear the result object to avoid memory leaks
        return NULL;
    }

    // Return the result to the caller
    return res; // Caller is responsible for clearing the PGresult
}

PGresult *is_group_updated(PGconn *conn, int chat_id, time_t timestamp) {
    const char *query =
        "SELECT chat_id, group_name "
        "FROM group_chats "
        "WHERE chat_id = $1 AND updated_at > to_timestamp($2)";

    char chat_id_str[12];
    char timestamp_str[20];

    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);
    snprintf(timestamp_str, sizeof(timestamp_str), "%ld", timestamp);

    const char *params[2] = {chat_id_str, timestamp_str};

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error checking group updates: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return NULL; // No updates found
    }

    return res; // Caller is responsible for freeing with PQclear
}

