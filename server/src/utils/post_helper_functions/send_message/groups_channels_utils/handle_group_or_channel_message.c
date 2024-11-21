#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../../../../../libraries/cJSON/cJSON.h"
#include "../../../../../inc/utils.h"

MessageResult_t store_message_return_message_info(PGconn *conn, int chat_id, int sender_id, const char *message_text,
                                  int media_id, int reply_to_chat, int reply_to_message,
                                  int forwarded_from_chat, int forwarded_from_message) {
    MessageResult_t result = { .Success = 0, .message_id = -1, .timestamp = "" };

    const char *query =
        "INSERT INTO messages (chat_id, sender_id, message_text, media, reply_to_chat, reply_to_message, "
        "forwarded_from_chat, forwarded_from_message) VALUES ($1, $2, $3, $4, $5, $6, $7, $8) "
        "RETURNING message_id, timestamp";

    char chat_id_str[12], sender_id_str[12], media_id_str[12], reply_to_chat_str[12], reply_to_message_str[12],
        forwarded_from_chat_str[12], forwarded_from_message_str[12];
    const char *params[8] = {
        itoa(chat_id, chat_id_str),
        itoa(sender_id, sender_id_str),
        message_text,
        media_id ? itoa(media_id, media_id_str) : NULL,
        reply_to_chat ? itoa(reply_to_chat, reply_to_chat_str) : NULL,
        reply_to_message ? itoa(reply_to_message, reply_to_message_str) : NULL,
        forwarded_from_chat ? itoa(forwarded_from_chat, forwarded_from_chat_str) : NULL,
        forwarded_from_message ? itoa(forwarded_from_message, forwarded_from_message_str) : NULL
    };

    PGresult *res = PQexecParams(conn, query, 8, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error: Failed to store group message in database: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return result;
    }

    // Extract message_id and timestamp
    result.message_id = atoi(PQgetvalue(res, 0, 0));
    strncpy(result.timestamp, PQgetvalue(res, 0, 1), sizeof(result.timestamp) - 1);
    result.Success = 1;

    PQclear(res);
    return result;
}


// Helper function: Get all recipients of a group or channel chat
PGresult *get_group_recipients(PGconn *conn, int chat_id) {
    const char *query = "SELECT user_id, role FROM group_chat_members WHERE chat_id = $1 AND role != 'banned' AND role != 'left'";
    const char *params[1];

    char chat_id_str[12];
    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);
    params[0] = chat_id_str;

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error: Failed to fetch group recipients: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }
    return res;
}

void deliver_message_to_online_user(int recipient_id, cJSON *json_message) {
    _send_message_to_client(recipient_id, json_message);
}

MessageResult_t handle_group_or_channel_message(PGconn *conn, int chat_id, int sender_id, const char *message_text,
                                    int media_id, int reply_to_chat, int reply_to_message, int forwarded_from_chat, int forwarded_from_message) {
    MessageResult_t result = store_message_return_message_info(conn, chat_id, sender_id, message_text, media_id, reply_to_chat, reply_to_message, forwarded_from_chat, forwarded_from_message);
    // Store the message in the database
    if (!result.Success) {
        return result;
    }

    // Get recipients for the group or channel
    PGresult *recipients = get_group_recipients(conn, chat_id);
    if (!recipients) {
        result.Success = -2;
        return result;
    }

    int recipient_count = PQntuples(recipients);
    for (int i = 0; i < recipient_count; i++) {
        int recipient_id = atoi(PQgetvalue(recipients, i, 0));
        const char *role = PQgetvalue(recipients, i, 1);

        if (strcmp(role, "restricted") == 0) {
            continue;
        }

        if (is_user_online(recipient_id)) {
            cJSON *json_message = create_message_json(sender_id, message_text);
            deliver_message_to_online_user(recipient_id, json_message);
            cJSON_Delete(json_message);
        } else {
            // TODO: Handle offline message delivery for group or channel
        }
    }

    PQclear(recipients);
    return result;
}





