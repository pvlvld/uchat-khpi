#ifndef MODAL_H
#define MODAL_H

#pragma once

#include "header.h"

typedef struct {
    GtkWidget *window;
    void (*show)(GtkWindow *parent, int x, int y, t_chat_info *chat_info);
    void (*destroy)(void);
} t_modal_chat_info;

typedef struct {
    GtkWidget *window;
    void (*show)(GtkWindow *parent, t_message_info_modal *message_info);
    void (*destroy)(void);
    int is_open;
} t_modal_edit_modal;

typedef struct {
    GtkWidget *window;
    void (*show)(GtkWindow *parent);
    void (*destroy)(void);
} t_modal_profile_settings;

typedef struct {
    GtkWidget *userblock;
    int user_id;
    int chat_id;
} t_user_block_data;

typedef struct {
    GtkWidget *window;
    void (*show)(GtkWindow *parent, int user_count, t_users_struct **users, int chat_id);
    void (*destroy)(void);
} t_modal_add_users_to_group;

typedef struct {
    GtkWidget *window;
    void (*show)(GtkWindow *parent, t_message_info_struct *message_info_struct, int x, int y, const char *text, gboolean is_full);
    void (*destroy)(void);
    t_modal_edit_modal edit_modal;
} t_modal_message_info;

typedef struct {
    GtkWidget *window;
    void (*show)(GtkWindow *parent);
    void (*destroy)(void);
} t_modal_create_group;

typedef struct {
    GtkWidget *window;
    void (*show)(GtkWindow *parent);
    void (*destroy)(void);
} t_modal_add_friend;

typedef struct {
    t_modal_chat_info chat_info;
    t_modal_profile_settings profile_settings;
    t_modal_message_info message_info;
    t_modal_create_group create_group;
    t_modal_add_friend add_friend;
    t_modal_add_users_to_group add_users_to_group;
} t_modal;

t_modal_chat_info init_modal_chat_info(void);
t_modal_profile_settings init_modal_profile_settings(void);
t_modal_edit_modal init_edit_modal(void);
t_modal_message_info init_modal_message_info(void);
t_modal_create_group init_modal_create_group(void);
t_modal_add_users_to_group init_modal_add_users_to_group(void);
t_modal_add_friend init_modal_add_friend(void);
t_modal init_modal(void);

#endif //MODAL_H
