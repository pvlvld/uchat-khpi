#ifndef ACTIVE_USERS_TABLE_H
#define ACTIVE_USERS_TABLE_H

#include "database.h"

typedef struct {
    int user_id;
    char *username;
    char *user_login;
    char *about;
    char *public_key;
    char *private_key;
} t_active_users_struct;

typedef struct {
    void (*create_table)(void);
    void (*add_user)(t_active_users_struct *user);
    t_active_users_struct *(*get_user_by_user_login)(const char *user_login);
    void (*free_struct)(t_active_users_struct *user);
} t_active_users_table;
t_active_users_table init_active_users_table(void);

#endif //ACTIVE_USERS_TABLE_H
