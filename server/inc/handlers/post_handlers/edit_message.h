#ifndef UCHAT_EDIT_MESSAGE_H
#define UCHAT_EDIT_MESSAGE_H

#include "../../utils.h"
#include "../../header.h"

typedef struct {
    bool Error;
    char *error_code;
    int message_id;        // ID of the message being edited
    char *timestamp;    // Timestamp of the last operation
    char *new_message_text;
} EditMessageResult_t;

EditMessageResult_t perform_message_edit(PGconn *conn, int chat_id, int req_sender_id, int message_id, char *new_message_text_str);
void send_edit_message_response(SSL *ssl, EditMessageResult_t *edit_message_result, cJSON *response_json);
bool send_ws_edit_message(PGconn *conn, int chat_id, int sender_id, EditMessageResult_t result);

void protected_edit_message_rout(SSL *ssl, const char *request);

#endif // UCHAT_EDIT_MESSAGE_H


