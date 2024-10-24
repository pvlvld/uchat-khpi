#ifndef HEADER_H
#define HEADER_H

#include <gtk/gtk.h>
#include "components/components.h"
#include "sidebar.h"

GtkWidget *init_sidebar(void);

typedef struct {
    t_components components;
} t_vendor;

extern t_vendor vendor;
void init_vendor(t_vendor *vendor);

#endif //HEADER_H
