#include "../../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

GtkWidget *create_register_username_wrapper(void) {
    t_register_form_data *form_data = &vendor.pages.register_page.form_data;

    GtkWidget *register_username_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.pages.register_page.username_wrapper = register_username_wrapper;
    gtk_widget_set_size_request(register_username_wrapper, -1, -1);
    vendor.helpers.set_classname_and_id(register_username_wrapper, "auth__username_wrapper");
    gtk_style_context_add_class(gtk_widget_get_style_context(register_username_wrapper), "_form__input_wrapper");

    GtkWidget *register_username = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(register_username, "auth__username");
    gtk_style_context_add_class(gtk_widget_get_style_context(register_username), "_form__input_block");
    gtk_widget_set_size_request(register_username, -1, 34);
    gtk_widget_set_halign(register_username, GTK_ALIGN_FILL);

    GtkWidget *register_username_image_wrapper = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(register_username_image_wrapper, 32, -1);
    vendor.helpers.set_classname_and_id(register_username_image_wrapper, "auth__username_image__wrapper");
    gtk_style_context_add_class(gtk_widget_get_style_context(register_username_image_wrapper), "_form__image_wrapper");
    gtk_box_pack_start(GTK_BOX(register_username), register_username_image_wrapper, FALSE, FALSE, 0);

    GtkWidget *register_username_image = gtk_image_new_from_file("resources/images/static/username.svg");
    gtk_widget_set_size_request(register_username_image, 12, 12);
    gtk_box_pack_start(GTK_BOX(register_username_image_wrapper), register_username_image, TRUE, TRUE, 0);

    GtkWidget *username_entry = gtk_entry_new();
    vendor.helpers.set_classname_and_id(username_entry, "auth__username_input");
    gtk_style_context_add_class(gtk_widget_get_style_context(username_entry), "_form__input");
    gtk_box_pack_start(GTK_BOX(register_username), username_entry, TRUE, TRUE, 0);
    form_data->username_entry = username_entry;

    GtkWidget *username_placeholder = gtk_label_new("Username");
    gtk_style_context_add_class(gtk_widget_get_style_context(username_placeholder), "_form__input_placeholder");
    gtk_widget_set_halign(username_placeholder, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(register_username_wrapper), register_username, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(register_username_wrapper), username_placeholder, TRUE, FALSE, 0);

    g_signal_connect(username_entry, "focus-in-event", G_CALLBACK(vendor.pages.register_page.functions.on_from_entry_focus_in), username_placeholder);
    g_signal_connect(username_entry, "focus-out-event", G_CALLBACK(vendor.pages.register_page.functions.on_from_entry_focus_out), username_placeholder);
    g_signal_connect(username_entry, "key-press-event", G_CALLBACK(vendor.pages.register_page.functions.on_key_press), form_data);
    gtk_widget_show(username_placeholder);

    return register_username_wrapper;
}
