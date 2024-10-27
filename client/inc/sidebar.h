#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "header.h"

typedef struct {
    unsigned int id;
    char *name;
    char *last_message;
    char *sender_name;
    int type;
    char *path_to_logo;
    int unreaded_messages;
    time_t timestamp;
} t_chat_info;

char *format_timestamp(time_t timestamp);
int compare_chats(const void *a, const void *b);

GtkWidget *sidebar_init(void);
GtkWidget *sidebar_create_avatar(t_chat_info *chat_info);

typedef struct {
    GtkWidget *(*init)(void);
    GtkWidget *(*create_avatar)(t_chat_info *chat_info);
} t_sidebar;

t_sidebar init_sidebar(void);

#endif //SIDEBAR_H
