#ifndef USERS_TABLE_H
#define USERS_TABLE_H

#include "database.h"

typedef struct {
    int user_id;
    char *username;
    char *user_login;
    char *about;
    int is_online;
    char *public_key;
    struct tm updated_at;
} t_users_struct;

typedef struct {
    void (*create_table)(void);
    t_users_struct *(*get_user_by_id)(int user_id);
    void (*free_struct)(t_users_struct *user);
} t_users_table;
t_users_table init_users_table(void);

#endif //USERS_TABLE_H
