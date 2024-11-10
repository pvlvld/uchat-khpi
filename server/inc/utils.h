#ifndef UTILS_H
#define UTILS_H

char *itoa(int value, char *buffer);
char *hash_password(const char *password);
bool is_valid_public_key(const char *public_key_str);

#endif // UTILS_H
