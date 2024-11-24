#ifndef UCHAT_DELETE_MESSAGE_H
#define UCHAT_DELETE_MESSAGE_H

#include "../../utils.h"
#include "../../header.h"

typedef struct {
    int Success;       // Indicates result (-1: not sender, 0: deletion failed, 1: success)
    int message_id;    // ID of the deleted message
    char timestamp[20]; // Timestamp of the successful deletion (format: "YYYY-MM-DD HH:MM:SS")
} deleteMessageResult_t;

deleteMessageResult_t delete_message_db_and_return_data(PGconn *conn, int chat_id, int message_id, int sender_id, bool any_delete_allowed);

#endif // UCHAT_DELETE_MESSAGE_H
