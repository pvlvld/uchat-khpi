#ifndef SIDEBAR_H
#define SIDEBAR_H

#pragma once

#include "header.h"

char *format_timestamp(time_t timestamp);
int compare_chats(const void *a, const void *b);

GtkWidget *sidebar_init(void);
GtkWidget *sidebar_create_avatar(t_chat_info *chat_info);
GtkWidget *sidebar_create_chatblock(t_chat_info *chat_info);


typedef struct {
    GtkWidget *(*init)(void);
    GtkWidget *(*create_chatblock)(t_chat_info *chat_info);
    GtkWidget *(*create_avatar)(t_chat_info *chat_info);
} t_sidebar;

t_sidebar init_sidebar(void);

#endif //SIDEBAR_H
