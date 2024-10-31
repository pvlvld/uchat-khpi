#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

GtkWidget *create_main_page(void) {
    GtkWidget *main_page = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *sidebar = vendor.sidebar.init();
    GtkWidget *right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *label = gtk_label_new("Select a chat to start messaging");

    gtk_widget_set_halign(label, GTK_ALIGN_FILL);
    gtk_widget_set_valign(label, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_vexpand(label, TRUE);

    gtk_box_pack_start(GTK_BOX(right_box), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_page), sidebar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_page), right_box, TRUE, TRUE, 0);

    vendor.helpers.set_classname_and_id(right_box, "_right_block");

    return main_page;
}
