#ifndef MODAL_H
#define MODAL_H

#pragma once

#include "header.h"

typedef struct {
    GtkWidget *window;
    void (*show)(GtkWindow *parent, int x, int y);
    void (*destroy)(void);
} t_modal_chat_ifno;

typedef struct {
    t_modal_chat_ifno chat_info;
} t_modal;

t_modal_chat_ifno init_modal_chat_info(void);
t_modal init_modal(void);

#endif //MODAL_H
