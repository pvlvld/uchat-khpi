#ifndef HEADER_H
#define HEADER_H

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "components/components.h"
#include "sidebar.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

typedef struct {
    t_components components;
    t_sidebar sidebar;
} t_vendor;

extern t_vendor vendor;
t_chat_info **parse_chats_info(void);
void free_chats_info(t_chat_info **chats_info);
void init_vendor(t_vendor *vendor);

#endif //HEADER_H
