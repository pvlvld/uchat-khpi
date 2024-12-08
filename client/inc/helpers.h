#ifndef HELPERS_H
#define HELPERS_H

#include "header.h"

typedef struct {
    void (*set_classname_and_id)(GtkWidget *widget, char *name);
    void (*show_notification)(const char *title, const char *message);
    void (*add_hover)(GtkWidget *event_box);
    char *(*strdup)(const char *s);
    GtkWidget *(*create_avatar)(const char *path, int width, int height);
    GtkWidget *(*create_image)(const char *path, GtkWidget *message, int width);
} t_helpers;

void set_classname_and_id(GtkWidget *widget, char *name);
void ubuntu_show_notification(const char *title, const char *message);
void macos_show_notification(const char *title, const char *message);
void show_notification(const char *title, const char *message); // Declaration only
GtkWidget *create_avatar(const char *path, int width, int height);
GtkWidget *create_image(const char *path, GtkWidget *message, int width);
char *mx_strdup(const char *s);

void add_hover(GtkWidget *event_box);

t_helpers init_helpers(void);

#endif //HELPERS_H
