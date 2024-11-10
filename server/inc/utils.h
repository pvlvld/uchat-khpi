#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

char *itoa(int value, char *buffer);
char *hash_password(const char *password);
bool is_valid_public_key(const char *public_key_str);

bool is_valid_login(const char *login);
bool is_valid_password(const char *password);

#endif // UTILS_H
