#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../../../../../libraries/cJSON/cJSON.h"
#include "../../../../../inc/utils.h"

int store_group_message(PGconn *conn, int chat_id, int sender_id, const char *message_text,
                        int media_id, int reply_to_chat, int reply_to_message, int forwarded_from_chat, int forwarded_from_message) {
    if (!send_message(conn, chat_id, sender_id, message_text, media_id, reply_to_chat, reply_to_message, forwarded_from_chat, forwarded_from_message)) {
        fprintf(stderr, "Error: Failed to store group message in database\n");
        return -1;
    }
    return 1;
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

int handle_group_or_channel_message(PGconn *conn, int chat_id, int sender_id, const char *message_text,
                                    int media_id, int reply_to_chat, int reply_to_message, int forwarded_from_chat, int forwarded_from_message) {
    // Store the message in the database
    if (store_group_message(conn, chat_id, sender_id, message_text, media_id, reply_to_chat, reply_to_message, forwarded_from_chat, forwarded_from_message) < 0) {
        return -1;
    }

    // Get recipients for the group or channel
    PGresult *recipients = get_group_recipients(conn, chat_id);
    if (!recipients) {
        return -2;
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
    return 1;
}





