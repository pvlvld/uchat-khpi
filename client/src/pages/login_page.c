#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

static gboolean on_link_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    (void) widget;
    (void) data;
    if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
        vendor.pages.change_page(REGISTER_PAGE);
        return TRUE;
    }
    return FALSE;
}

GtkWidget *create_login_page(void) {
    gtk_window_set_title(GTK_WINDOW(vendor.window), "ShadowTalk | Login");
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    GtkWidget *wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(wrapper, "login");
    gtk_widget_set_halign(wrapper, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(wrapper, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(wrapper, TRUE);
    gtk_widget_set_vexpand(wrapper, TRUE);

    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(content, 380, -1);
    gtk_widget_set_hexpand(content, FALSE);

    vendor.helpers.set_classname_and_id(content, "auth__content");
    GtkWidget *login_block = login_init();

    GtkWidget *title = vendor.components.title.create_h1("Login");
    vendor.helpers.set_classname_and_id(title, "auth__title");

    GtkWidget *label_text = gtk_label_new("Don’t have an account? ");
    vendor.helpers.set_classname_and_id(label_text, "auth__text");

    GtkWidget *event_box = gtk_event_box_new();
    GtkWidget *label_link = gtk_label_new("Register");
    vendor.helpers.set_classname_and_id(label_link, "auth__link");

    gtk_container_add(GTK_CONTAINER(event_box), label_link);
    gtk_widget_set_events(event_box, GDK_BUTTON_PRESS_MASK);

    g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_link_clicked), NULL);
    vendor.helpers.add_hover(event_box);

    GtkWidget *label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(label_box), label_text, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(label_box), event_box, FALSE, FALSE, 0);
    gtk_widget_set_halign(label_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label_box, GTK_ALIGN_CENTER);
    vendor.helpers.set_classname_and_id(label_box, "auth__label-box");

    gtk_box_pack_start(GTK_BOX(content), title, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(content), login_block, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(content), label_box, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(wrapper), content, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(page), wrapper, FALSE, FALSE, 0);

    vendor.helpers.set_classname_and_id(page, "_right_block");

    return page;
}
