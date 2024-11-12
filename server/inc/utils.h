#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include "header.h"

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


#endif // UTILS_H
