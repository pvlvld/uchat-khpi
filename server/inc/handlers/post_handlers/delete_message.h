#ifndef UCHAT_DELETE_MESSAGE_H
#define UCHAT_DELETE_MESSAGE_H

#include "../../utils.h"
#include "../../header.h"

deleteMessageResult_t check_if_message_already_deleted(PGconn *conn, int chat_id, int message_id);
deleteMessageResult_t delete_message_db_and_return_data(PGconn *conn, int chat_id, int message_id, int sender_id, bool any_delete_allowed);

#endif // UCHAT_DELETE_MESSAGE_H
