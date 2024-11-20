#ifndef MODAL_H
#define MODAL_H

#pragma once

#include "header.h"

typedef struct {
    GtkWidget *window;
    void (*show)(GtkWindow *parent, int x, int y);
    void (*destroy)(void);
} t_modal_chat_info;

typedef struct {
    GtkWidget *window;
    void (*show)(GtkWindow *parent);
    void (*destroy)(void);
} t_modal_profile_settings;

typedef struct {
    GtkWidget *window;
    void (*show)(GtkWindow *parent, int x, int y);
    void (*destroy)(void);
} t_modal_message_info;

typedef struct {
    t_modal_chat_info chat_info;
    t_modal_profile_settings profile_settings;
    t_modal_message_info message_info;
} t_modal;

t_modal_chat_info init_modal_chat_info(void);
t_modal_profile_settings init_modal_profile_settings(void);
t_modal_message_info init_modal_message_info(void);
t_modal init_modal(void);

#endif //MODAL_H
