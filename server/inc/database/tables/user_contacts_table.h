#ifndef USER_CONTACTS_TABLE_H
#define USER_CONTACTS_TABLE_H
#pragma once

#include <libpq-fe.h>
#include <stdbool.h>


/** Creates a user.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param username The username of the user.
 * @param user_login The login of the user.
 * @param password_hash The hash of the user's password.
 * @param public_key The public encription key of the user.
 * @param locale The locale of the user.
 * @return The ID of the created user, or -1 if the user could not be created.
 */
int create_user(PGconn *conn, const char *username, const char *user_login, const char *password_hash, const char *salt, const char *public_key, const char *locale);

/**
 * Sends a friend request.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user sending the request.
 * @param contact_id The ID of the user receiving the request.
 * @return true if the request was sent successfully, false otherwise.
 */
bool send_friend_request(PGconn *conn, int user_id, int contact_id);

/**
 * Accepts a friend request.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user accepting the request.
 * @param contact_id The ID of the user who sent the request.
 * @return true if the request was accepted successfully, false otherwise.
 */
bool accept_friend_request(PGconn *conn, int user_id, int contact_id);

/** Rejects a friend request.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user rejecting the request.
 * @param contact_id The ID of the user who sent the request.
 * @return true if the request was rejected successfully, false otherwise.
 */
bool reject_friend_request(PGconn *conn, int user_id, int contact_id);

/** Deletes a friend.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user deleting the friend.
 * @param contact_id The ID of the friend to delete.
 * @return true if the friend was deleted successfully, false otherwise.
 */
bool delete_friend(PGconn *conn, int user_id, int contact_id);

/** Checks if two users are friends.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the first user.
 * @param contact_id The ID of the second user.
 * @return true if the users are friends, false otherwise.
 */
bool is_friend(PGconn *conn, int user_id, int contact_id);

typedef struct {
    int (*create_user)(PGconn *conn, const char *username, const char *user_login, const char *password_hash, const char *salt, const char *public_key, const char *locale);
    bool (*send_friend_request)(PGconn *conn, int user_id, int contact_id);
    bool (*accept_friend_request)(PGconn *conn, int user_id, int contact_id);
    bool (*reject_friend_request)(PGconn *conn, int user_id, int contact_id);
    bool (*delete_friend)(PGconn *conn, int user_id, int contact_id);
    bool (*is_friend)(PGconn *conn, int user_id, int contact_id);
} t_user_contacts_table;

#endif // USER_CONTACTS_TABLE_H
