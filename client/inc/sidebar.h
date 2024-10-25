#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "header.h"

GtkWidget *sidebar_init(void);
GtkWidget *sidebar_create_avatar(int avatar_id, const gchar *image_path);

typedef struct {
    GtkWidget *(*init)(void);
    GtkWidget *(*create_avatar)(int avatar_id, const gchar *image_path);
} t_sidebar;

t_sidebar init_sidebar(void);

#endif //SIDEBAR_H
