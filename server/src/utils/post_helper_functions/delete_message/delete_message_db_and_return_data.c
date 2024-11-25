#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../../../../inc/handlers/post_handlers/delete_message.h"

deleteMessageResult_t check_if_message_already_deleted(PGconn *conn, int chat_id, int message_id) {
    deleteMessageResult_t result = { .Success = 0, .message_id = -1, .timestamp = { '\0' } };

    const char *query =
        "SELECT deleted_at "
        "FROM messages "
        "WHERE chat_id = $1 AND message_id = $2";

    char chat_id_str[12], message_id_str[12];
    const char *params[2] = {
        itoa(chat_id, chat_id_str),
        itoa(message_id, message_id_str)
    };

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error: Failed to check message deletion status: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return result;
    }

    if (PQntuples(res) == 0) {
        fprintf(stderr, "Error: No message found with chat_id=%d and message_id=%d\n", chat_id, message_id);
        PQclear(res);
        return result;
    }

    char *deleted_at_value = PQgetvalue(res, 0, 0);
    if (deleted_at_value && strlen(deleted_at_value) > 0) {
        // Message has already been deleted
        result.Success = 2; // Special success code indicating message was already deleted
        result.message_id = message_id;
        strncpy(result.timestamp, deleted_at_value, sizeof(result.timestamp) - 1);
    }

    PQclear(res);
    return result;
}

deleteMessageResult_t delete_message_db_and_return_data(PGconn *conn, int chat_id, int message_id, int sender_id, bool any_delete_allowed) {
    // Initialize result structure
    deleteMessageResult_t result = check_if_message_already_deleted(conn, chat_id, message_id);
    if (result.Success == 2) {
        return result;
    }
    // Retrieve the sender of the message
    MessageSenderResult_t message_sender = get_message_sender(conn, chat_id, message_id);

    // Compare retrieved sender with the provided sender_id
    if (!any_delete_allowed) {
        if (message_sender.sender_id != sender_id) {
            result.Success = -1; // Sender mismatch
            // user cannot delete this message because he's not the sender
            return result;
        }
    }

    // delete the message using the delete_message function
    if (!delete_message(conn, chat_id, message_id)) {
        result.Success = 0;
        // Delete message failed
        return result;
    }

    // Populate result on successful deletion
    result.Success = 1;
    result.message_id = message_id;

    const char *timestamp_query = "SELECT TO_CHAR(NOW(), 'YYYY-MM-DD HH24:MI:SS')";
    PGresult *timestamp_res = PQexec(conn, timestamp_query);

    if (PQresultStatus(timestamp_res) == PGRES_TUPLES_OK && PQntuples(timestamp_res) > 0) {
        strncpy(result.timestamp, PQgetvalue(timestamp_res, 0, 0), sizeof(result.timestamp) - 1);
    } else {
        strncpy(result.timestamp, "UNKNOWN", sizeof(result.timestamp) - 1); // Fallback value
    }

    PQclear(timestamp_res);

    // delete message succeed
    printf("Result: %d, Deleted message with ID: %d\n", result.Success, message_id);
    return result;
}


