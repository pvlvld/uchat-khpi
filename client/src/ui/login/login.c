#include "../../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

GtkWidget *login_init(void) {
    GtkWidget *block = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
    vendor.helpers.set_classname_and_id(block, "auth__block");
    gtk_widget_set_halign(block, GTK_ALIGN_FILL);
    gtk_widget_set_size_request(block, 308, 155);
    gtk_widget_set_hexpand(block, FALSE);

    GtkWidget *username_wrapper = vendor.pages.login_page.functions.create_username_wrapper();
    GtkWidget *password_wrapper = vendor.pages.login_page.functions.create_password_wrapper();
    GtkWidget *button = vendor.pages.login_page.functions.create_button();

    gtk_box_pack_start(GTK_BOX(block), username_wrapper, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(block), password_wrapper, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(block), button, TRUE, TRUE, 0);

    return block;
}

t_login_page init_login(void) {
    t_login_page login = {
        .init = login_init,
        .form_data = {
            .username_entry = NULL,
            .password_entry = NULL,
            .toggle_button = NULL,
            .toggle_image = NULL,
        },
        .username_wrapper = NULL,
        .password_wrapper = NULL,
        .username_error = NULL,
        .password_error = NULL,
        .functions = init_login_functions(),
    };

    return login;
}
