#include "../../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

GtkWidget *create_title_h1(const char *text) {
    GtkWidget *title = gtk_label_new(text);
    gtk_style_context_add_class(gtk_widget_get_style_context(title), "_title");

    return title;
}
