#ifndef HELPERS_H
#define HELPERS_H

#include "header.h"

typedef struct {
    void (*set_classname_and_id)(GtkWidget *widget, char *name);
    void (*show_notification)(const char *title, const char *message);
} t_helpers;

void set_classname_and_id(GtkWidget *widget, char *name);
void show_notification(const char *title, const char *message);
t_helpers init_helpers(void);

#endif //HELPERS_H
