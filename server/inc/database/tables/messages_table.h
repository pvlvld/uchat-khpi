#ifndef MESSAGES_TABLE_H
#define MESSAGES_TABLE_H
#pragma once

#include <libpq-fe.h>

/** Sends a message to a chat.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param chat_id The ID of the chat to send the message to.
 * @param sender_id The ID of the user sending the message.
 * @param message_text OPTIONAL. The text of the message.
 * @param media_id OPTIONAL. The ID of the media.
 * @param reply_to_chat OPTIONAL. The ID of the chat being replied to.
 * @param reply_to_message OPTIONAL. The ID of the message being replied to.
 * @param forwarded_from_chat OPTIONAL. The ID of the chat the message was forwarded from.
 * @param forwarded_from_message OPTIONAL. The ID of the message the message was forwarded from.
 * @return true if the message was sent successfully, false otherwise.
 */
bool send_message(PGconn *conn, int chat_id, int sender_id, const char *message_text, int media_id, int reply_to_chat,
                  int reply_to_message, int forwarded_from_chat, int forwarded_from_message);
/** Deletes a message from a chat.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param chat_id The ID of the chat to delete the message from.
 * @param message_id The ID of the message to delete.
 * @return true if the message was deleted successfully, false otherwise.
 */
bool delete_message(PGconn *conn, int chat_id, int message_id);

/** Edits the text of a message.
 * @param conn The connection to the PostgreSQL database.
 * @param chat_id The ID of the chat the message is in.
 * @param message_id The ID of the message to edit.
 * @param new_message_text The new text of the message.
 * @return true if the message was edited successfully, false otherwise.
 */
bool edit_message_text(PGconn *conn, int chat_id, int message_id, const char *new_message_text);

/** Retrieves a message from a chat.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param chat_id The ID of the chat the message is in.
 * @param message_id The ID of the message to retrieve.
 * @return A PGresult containing the message data, or NULL if the query failed.
 *         Caller is responsible for freeing the result with PQclear.
 */
PGresult *get_message(PGconn *conn, int chat_id, int message_id);

/** Retrieves messages from a chat.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param chat_id The ID of the chat to retrieve messages from.
 * @param limit The maximum number of messages to retrieve.
 * @param offset The number of messages to skip.
 * @return A PGresult containing the message data, or NULL if the query failed.
 *         Caller is responsible for freeing the result with PQclear.
 */
PGresult *get_messages(PGconn *conn, int chat_id, int limit, int offset);

typedef struct {
    bool (*send_message)(PGconn *conn, int chat_id, int sender_id, const char *message_text, int media_id,
                         int reply_to_chat, int reply_to_message, int forwarded_from_chat, int forwarded_from_message);
    bool (*delete_message)(PGconn *conn, int chat_id, int message_id);
    bool (*edit_message_text)(PGconn *conn, int chat_id, int message_id, const char *new_message_text);
    PGresult *(*get_message)(PGconn *conn, int chat_id, int message_id);
    PGresult *(*get_messages)(PGconn *conn, int chat_id, int limit, int offset);
} t_messages_table;

#endif // MESSAGES_TABLE_H
