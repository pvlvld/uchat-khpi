#ifndef UTILS_H
#define UTILS_H

// compatabiliby with c99 and c11 (strdup)
#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include "header.h"
#include <string.h>

char *itoa(int value, char *buffer);
char *mx_strdup(const char *s1);

char *hash_password(const char *password);

char *get_sender_from_token(const char *request);

bool is_valid_login(const char *login);
bool is_valid_password(const char *password);
bool is_valid_public_key(const char *public_key_str);

bool is_valid_username(const char *username);
char *username_validation(const char *username, const char *user_login, PGconn *conn);

// send message
char *extract_recipient_login(cJSON *json);
char *extract_message(cJSON *json);
bool is_valid_message(const char *message);
bool is_valid_chat_id(const char *chat_id_str);
bool is_user_in_chat(PGconn *conn, int chat_id, int user_id);
const char *get_user_role_in_group(PGconn *conn, int chat_id, int user_id);
char *extract_chat_id(cJSON *json);

// compatability
/* Duplicate S, returning an identical malloc'd string.  */
extern char *strdup (const char *__s)
     __THROW __attribute_malloc__ __nonnull ((1));

#endif // UTILS_H
