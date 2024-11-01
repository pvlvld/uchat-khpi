#ifndef PERSONAL_CHATS_TABLE_H
#define PERSONAL_CHATS_TABLE_H
#pragma once

#include <libpq-fe.h>
#include <stdbool.h>

/** Creates a personal chat between two users.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user1_id The ID of the first user.
 * @param user2_id The ID of the second user.
 * @return The ID of the created chat, or -1 if the chat could not be created.
 */
int create_personal_chat(PGconn *conn, int user1_id, int user2_id);

/** Retrieves a personal chat between two users.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user1_id The ID of the first user.
 * @param user2_id The ID of the second user.
 * @return A PGresult containing the chat data, or NULL if the query failed.
 *         Caller is responsible for freeing the result with PQclear.
 */
PGresult *get_personal_chat(PGconn *conn, int user1_id, int user2_id);

/** Deletes a personal chat between two users.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user1_id The ID of the first user.
 * @param user2_id The ID of the second user.
 * @return true if the chat was deleted successfully, false otherwise.
 */
bool delete_personal_chat(PGconn *conn, int user1_id, int user2_id);

/** Updates the background of a personal chat between two users.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user1_id The ID of the first user.
 * @param user2_id The ID of the second user.
 * @param media_id The ID of the media to set as the background.
 * @return true if the background was updated successfully, false otherwise.
 */
bool update_background_personal_chat(PGconn *conn, int user1_id, int user2_id, int media_id);

/** Lists all personal chats of a user.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user.
 * @return A PGresult containing the chat data, or NULL if the query failed.
 *         Caller is responsible for freeing the result with PQclear.
 */
PGresult *list_user_personal_chats(PGconn *conn, int user_id);

typedef struct {
    int (*create_personal_chat)(PGconn *conn, int user1_id, int user2_id);
    PGresult *(*get_personal_chat)(PGconn *conn, int user1_id, int user2_id);
    bool (*delete_personal_chat)(PGconn *conn, int user1_id, int user2_id);
    bool (*update_background_personal_chat)(PGconn *conn, int user1_id, int user2_id, int media_id);
    PGresult *(*list_user_personal_chats)(PGconn *conn, int user_id);
} t_personal_chat_table;

#endif // PERSONAL_CHATS_TABLE_H
