#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

GtkWidget *create_loading_page(void) {
    GtkWidget *login_page = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *label = gtk_label_new("Loading");

    gtk_widget_set_halign(label, GTK_ALIGN_FILL);
    gtk_widget_set_valign(label, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_vexpand(label, TRUE);

    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(login_page), box, TRUE, TRUE, 0);

    vendor.helpers.set_classname_and_id(login_page, "_right_block");

    return login_page;
}
