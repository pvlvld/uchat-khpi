#include "../../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

static void on_toggle_password_visibility(GtkToggleButton *toggle_button, gpointer data) {
    t_register_form_data *form_data = (t_register_form_data *)data;

    gboolean is_visible = gtk_toggle_button_get_active(toggle_button);
    gtk_entry_set_visibility(GTK_ENTRY(form_data->password_entry), is_visible);

    if (is_visible) {
        gtk_image_set_from_file(GTK_IMAGE(form_data->toggle_image), "resources/images/static/cross.svg");
    } else {
        gtk_image_set_from_file(GTK_IMAGE(form_data->toggle_image), "resources/images/static/eye.svg");
    }
}

GtkWidget *create_register_password_wrapper(void) {
    t_register_form_data *form_data = &vendor.pages.register_page.form_data;

    GtkWidget *register_password_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.pages.register_page.password_wrapper = register_password_wrapper;
    gtk_widget_set_size_request(register_password_wrapper, -1, -1);
    vendor.helpers.set_classname_and_id(register_password_wrapper, "auth__password_wrapper");
    gtk_style_context_add_class(gtk_widget_get_style_context(register_password_wrapper), "_form__input_wrapper");

    GtkWidget *register_password = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(register_password, "auth__password");
    gtk_style_context_add_class(gtk_widget_get_style_context(register_password), "_form__input_block");
    gtk_widget_set_size_request(register_password, -1, 34);

    GtkWidget *register_password_image_wrapper = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(register_password_image_wrapper, 32, -1);
    vendor.helpers.set_classname_and_id(register_password_image_wrapper, "auth__passowrd_image__wrapper");
    gtk_style_context_add_class(gtk_widget_get_style_context(register_password_image_wrapper), "_form__image_wrapper");
    gtk_box_pack_start(GTK_BOX(register_password), register_password_image_wrapper, FALSE, FALSE, 0);

    GtkWidget *register_password_image = gtk_image_new_from_file("resources/images/static/password.svg");
    gtk_widget_set_size_request(register_password_image, 12, 15);
    gtk_box_pack_start(GTK_BOX(register_password_image_wrapper), register_password_image, TRUE, TRUE, 0);

    GtkWidget *password_entry = gtk_entry_new();
    vendor.helpers.set_classname_and_id(password_entry, "auth__password_input");
    gtk_style_context_add_class(gtk_widget_get_style_context(password_entry), "_form__input");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_box_pack_start(GTK_BOX(register_password), password_entry, TRUE, TRUE, 0);
    form_data->password_entry = password_entry;

    GtkWidget *password_placeholder = gtk_label_new("Password");
    gtk_style_context_add_class(gtk_widget_get_style_context(password_placeholder), "_form__input_placeholder");
    gtk_widget_set_halign(password_placeholder, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(register_password_wrapper), register_password, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(register_password_wrapper), password_placeholder, TRUE, FALSE, 0);

    g_signal_connect(password_entry, "focus-in-event", G_CALLBACK(vendor.pages.register_page.functions.on_from_entry_focus_in), password_placeholder);
    g_signal_connect(password_entry, "focus-out-event", G_CALLBACK(vendor.pages.register_page.functions.on_from_entry_focus_out), password_placeholder);
    g_signal_connect(password_entry, "key-press-event", G_CALLBACK(vendor.pages.register_page.functions.on_key_press), form_data);
    gtk_widget_show(password_placeholder);

    GtkWidget *toggle_button = gtk_toggle_button_new();
    GtkWidget *toggle_image = gtk_image_new_from_file("resources/images/static/eye.svg");
    gtk_container_add(GTK_CONTAINER(toggle_button), toggle_image);
    form_data->toggle_button = toggle_button;
    form_data->toggle_image = toggle_image;

    gtk_box_pack_start(GTK_BOX(register_password), toggle_button, FALSE, FALSE, 0);
    gtk_widget_set_can_focus(toggle_button, FALSE);

    gtk_style_context_add_class(gtk_widget_get_style_context(toggle_button), "_form__input_toggle");

    g_signal_connect(toggle_button, "toggled", G_CALLBACK(on_toggle_password_visibility), form_data);
    vendor.helpers.add_hover(toggle_button);

    return register_password_wrapper;
}
