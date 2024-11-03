#include "../../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

GtkWidget *create_register_button(void) {
    GtkWidget *button = gtk_button_new_with_label("Register");
    vendor.helpers.set_classname_and_id(button, "auth__button");
    gtk_widget_set_size_request(button, -1, 40);
    g_signal_connect(button, "clicked", G_CALLBACK(vendor.pages.register_page.functions.on_register_submit), &vendor.pages.register_page.form_data);
    vendor.helpers.add_hover(button);

    return button;
}
