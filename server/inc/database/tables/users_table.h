#ifndef USERS_TABLE_H
#define USERS_TABLE_H
#pragma once

#include <libpq-fe.h>

int create_user(PGconn *conn, const char *username, const char *user_login, const char *password_hash,
                const char *public_key, const char *locale);

/**
 * Retrieves a user by their login.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_login The login of the user to retrieve.
 * @return A PGresult containing the user data, or NULL if the query failed.
 *         Caller is responsible for freeing the result with PQclear.
 */
PGresult *get_user_by_login(PGconn *conn, const char *user_login);

/**
 * Retrieves a user by their ID.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user to retrieve.
 * @return A PGresult containing the user data, or NULL if the query failed.
 *         Caller is responsible for freeing the result with PQclear.
 */
PGresult *get_user_by_id(PGconn *conn, int user_id);

/**
 * Updates the locale of a user.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user to update.
 * @param locale The new locale to set for the user.
 * @return true if the update was successful, false otherwise.
 */
bool update_user_locale(PGconn *conn, int user_id, const char *locale);

/**
 * Updates the username of a user.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user to update.
 * @param username The new username to set for the user.
 * @return true if the update was successful, false otherwise.
 */
bool update_user_username(PGconn *conn, int user_id, const char *username);

/**
 * Deletes the user.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user to delete.
 * @return true if deletion was successful, false otherwise.
 */
bool delete_user(PGconn *conn, int user_id);

/**
 * Restores deleted user.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user to restore.
 * @return true if restored successfully, false otherwise.
 */
bool restore_user(PGconn *conn, int user_id);

/**
 * Updates user password.
 *
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user to restore.
 * @return true if restored successfully, false otherwise.
 */
bool update_user_password(PGconn *conn, int user_id, const char *password_hash);

/**
 * Updates user about.
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user to restore.
 * @param about The new about to set for the user.
 * @return true if restored successfully, false otherwise.
 */
bool update_user_about(PGconn *conn, int user_id, const char *about);

/**
 * Updates user profile picture.
 * @param conn The connection to the PostgreSQL database.
 * @param user_id The ID of the user to restore.
 * @param profile_picture_id The new profile picture to set for the user.
 * @return true if restored successfully, false otherwise.
 */
bool update_user_profile_picture(PGconn *conn, int user_id, int profile_picture_id);

#endif // USERS_TABLE_H
