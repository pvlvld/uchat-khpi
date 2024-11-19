#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../../../../../libraries/cJSON/cJSON.h"
#include "../../../../../inc/utils.h"
#include "../../../../../inc/database/tables/messages_table.h"

// Revised function to handle sending a message in a personal chat
int handle_personal_chat_message(PGconn *conn, int chat_id, int sender_id, int recipient_id, const char *message_text,
                                  int media_id, int reply_to_chat, int reply_to_message, int forwarded_from_chat, int forwarded_from_message) {
    // Step 1: Store the message in the database using send_message
    if (!send_message(conn, chat_id, sender_id, message_text, media_id, reply_to_chat, reply_to_message, forwarded_from_chat, forwarded_from_message)) {
        fprintf(stderr, "Error: Failed to store message in database\n");
        return -1;
    }

    // Step 2: Check if the recipient is online
    if (!is_user_online(recipient_id)) {
        // TODO: Handle offline message delivery
        return 2;
    }

    // Step 3: If the recipient is online, send the message via WebSocket
    cJSON *json_message = create_message_json(sender_id, message_text);
    _send_message_to_client(recipient_id, json_message);
    cJSON_Delete(json_message);
    return 1;
}
