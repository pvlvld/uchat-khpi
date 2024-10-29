#include "../../inc/header.h"
#include <gtk/gtk.h>

void set_classname_and_id(GtkWidget *widget, char *name) {
    gtk_widget_set_name(widget, name);
    gtk_style_context_add_class(gtk_widget_get_style_context(widget), name);
}
