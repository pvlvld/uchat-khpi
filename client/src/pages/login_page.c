#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

GtkWidget *create_login_page(void) {
    GtkWidget *login_page = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    GtkWidget *login = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(login, "login");
    gtk_widget_set_halign(login, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(login, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(login, TRUE);
    gtk_widget_set_vexpand(login, TRUE);

    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(content, 380, -1);
    gtk_widget_set_hexpand(content, FALSE);

    vendor.helpers.set_classname_and_id(content, "login__content");
    GtkWidget *login_block = login_init();

    GtkWidget *title = vendor.components.title.create_h1("Login");
    vendor.helpers.set_classname_and_id(title, "login__title");

    gtk_box_pack_start(GTK_BOX(content), title, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(content), login_block, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(login), content, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(login_page), login, FALSE, FALSE, 0);

    vendor.helpers.set_classname_and_id(login_page, "_right_block");

    return login_page;
}
