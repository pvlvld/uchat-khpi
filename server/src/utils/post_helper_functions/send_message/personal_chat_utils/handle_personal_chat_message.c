#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../../../../../libraries/cJSON/cJSON.h"
#include "../../../../../inc/utils.h"
#include "../../../../../inc/database/tables/messages_table.h"

// Revised function to handle sending a message in a personal chat
MessageResult_t handle_personal_chat_message(PGconn *conn, int chat_id, int sender_id, int recipient_id, const char *message_text,
                                  int media_id, int reply_to_chat, int reply_to_message, int forwarded_from_chat, int forwarded_from_message) {
    // Step 1: Store the message in the database using send_message
    MessageResult_t result = store_message_return_message_info(conn, chat_id, sender_id, message_text, media_id, reply_to_chat, reply_to_message, forwarded_from_chat, forwarded_from_message);

    if (result.Success == 0) {
        fprintf(stderr, "Error: Failed to store message in database\n");
        result.Success = -1;
        return result;
    }

    // Step 2: Check if the recipient is online
    if (!is_user_online(recipient_id)) {
        // TODO: Handle offline message delivery
        result.Success = 2;
        return result;
    }

    // Step 3: If the recipient is online, send the message via WebSocket
    cJSON *json_message = create_message_json(sender_id, "Send message");
    if (json_message == NULL) {
        result.Success = -2;
        cJSON_Delete(json_message);
        return result;
    }
    cJSON_AddStringToObject(json_message, "message_text", message_text);
    cJSON_AddNumberToObject(json_message, "chat_id", chat_id);
    cJSON_AddNumberToObject(json_message, "message_id", result.message_id);
    cJSON_AddStringToObject(json_message, "timestamp", result.timestamp);

    _send_message_to_client(recipient_id, json_message);
    cJSON_Delete(json_message);
    return result;
}
