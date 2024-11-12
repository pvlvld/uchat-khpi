#ifndef CHAT_H
#define CHAT_H

#include "header.h"

typedef struct {
    GtkWidget *(*init)(void);
    GtkWidget *chat_box;
    GtkWidget *scrolled_window;
    GtkWidget *stretchable_box_old_messages;
    GtkWidget *stretchable_box_new_messages;
    int static_height;
} t_chat;

typedef struct {
    GtkAdjustment *adjustment;
    double height;
} t_scroll_data;

t_chat init_chat(void);
void add_chat_message(const char *message_txt);

#endif //CHAT_H
