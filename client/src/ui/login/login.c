#include "../../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

static void create_error(GtkWidget *widget, const char *message) {
    GtkWidget *error = gtk_label_new(message);
    GtkWidget *error_wrapper = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(error_wrapper, GTK_ALIGN_START);

    gtk_label_set_line_wrap(GTK_LABEL(error), TRUE);
    gtk_label_set_lines(GTK_LABEL(error), 2);
    gtk_label_set_ellipsize(GTK_LABEL(error), PANGO_ELLIPSIZE_END);
    gtk_label_set_max_width_chars(GTK_LABEL(error), 40);
    gtk_widget_set_halign(error, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(error_wrapper), error, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(widget), error_wrapper, FALSE, FALSE, 0);

    gtk_widget_show_all(error_wrapper);
}

static gboolean validate_input(const gchar *username, const gchar *password) {
    gboolean is_valid = TRUE;

    GList *usrname_children = gtk_container_get_children(GTK_CONTAINER(vendor.pages.login_page.username_wrapper));
    if (g_list_length(usrname_children) != 1) {
        GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(usrname_children, 1));
        gtk_widget_destroy(target_child);
    }

    GList *password_children = gtk_container_get_children(GTK_CONTAINER(vendor.pages.login_page.password_wrapper));
    if (g_list_length(password_children) != 1) {
        GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(password_children, 1));
        gtk_widget_destroy(target_child);
    }

    if (g_strcmp0(username, "") == 0 || g_utf8_strlen(username, -1) < 3) {
//        *username_error = gtk_label_new("Username must be at least 3 characters long.");
        create_error(vendor.pages.login_page.username_wrapper, "Username must be at least 3 characters long");
        is_valid = FALSE;
    } else if (!g_regex_match_simple("^[a-zA-Z0-9_]+$", username, 0, 0)) {
//        *username_error = gtk_label_new("Username can only contain letters, digits, and underscores.");
        create_error(vendor.pages.login_page.username_wrapper, "Username can only contain letters, digits, and underscores");
        is_valid = FALSE;
    }

    // Проверка пароля
    if (g_strcmp0(password, "") == 0 || strlen(password) < 8) {
//        *password_error = gtk_label_new("Password must be at least 8 characters long.");
        create_error(vendor.pages.login_page.password_wrapper, "Password must be at least 8 characters long");
        is_valid = FALSE;
    } else if (!g_regex_match_simple(".*[A-Za-z].*", password, 0, 0)) {
//        *password_error = gtk_label_new("Password must contain at least one letter.");
        create_error(vendor.pages.login_page.password_wrapper, "Password must contain at least one letter");
        is_valid = FALSE;
    } else if (!g_regex_match_simple(".*[0-9].*", password, 0, 0)) {
//        *password_error = gtk_label_new("Password must contain at least one digit.");
        create_error(vendor.pages.login_page.password_wrapper, "Password must contain at least one digit");
        is_valid = FALSE;
    } else if (!g_regex_match_simple(".*[@#$%^&*()/\\-+=!<>?].*", password, 0, 0)) {
//        *password_error = gtk_label_new("Password must contain at least one special character (e.g., @, #, $, %, &).");
        create_error(vendor.pages.login_page.password_wrapper, "Password must contain at least one special character (e.g., @, #, $, %, &)");
        is_valid = FALSE;
    }

    return is_valid;
}

static void on_login_submit(GtkButton *button, gpointer user_data) {
    (void)button;
    t_login_form_data *data = (t_login_form_data *)user_data;

    const gchar *username = gtk_entry_get_text(GTK_ENTRY(data->username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(data->password_entry));

    gboolean is_valid = validate_input(username, password);

    if (!is_valid) {
        g_print("Invalid input detected.\n");
        return;
    }

    g_print("Login: %s\nPassword: %s\n", username, password);
}

static void on_toggle_password_visibility(GtkToggleButton *toggle_button, gpointer data) {
    t_login_form_data *form_data = (t_login_form_data *)data;

    gboolean is_visible = gtk_toggle_button_get_active(toggle_button);
    gtk_entry_set_visibility(GTK_ENTRY(form_data->password_entry), is_visible);

    if (is_visible) {
        gtk_image_set_from_file(GTK_IMAGE(form_data->toggle_image), "resources/images/static/cross.svg");
    } else {
        gtk_image_set_from_file(GTK_IMAGE(form_data->toggle_image), "resources/images/static/eye.svg");
    }
}

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    (void)widget;
    if (event->keyval == GDK_KEY_Return) {
        on_login_submit(NULL, user_data);
        return TRUE;
    }
    return FALSE;
}

GtkWidget *login_init(void) {
    t_login_form_data *form_data = &vendor.pages.login_page.form_data;

    GtkWidget *login_block = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
    vendor.helpers.set_classname_and_id(login_block, "login__block");
    gtk_widget_set_halign(login_block, GTK_ALIGN_FILL);
    gtk_widget_set_size_request(login_block, 308, 155);
    gtk_widget_set_hexpand(login_block, FALSE);

    GtkWidget *login_username_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.pages.login_page.username_wrapper = login_username_wrapper;
    gtk_widget_set_size_request(login_username_wrapper, -1, -1);
    vendor.helpers.set_classname_and_id(login_username_wrapper, "login__username_wrapper");

    GtkWidget *login_username = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(login_username, "login__username");
    gtk_widget_set_size_request(login_username, -1, 34);

    GtkWidget *username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Enter Username");
    gtk_box_pack_start(GTK_BOX(login_username), username_entry, TRUE, TRUE, 0);
    form_data->username_entry = username_entry;

    gtk_box_pack_start(GTK_BOX(login_username_wrapper), login_username, TRUE, TRUE, 0);

    GtkWidget *login_password_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.pages.login_page.password_wrapper = login_password_wrapper;
    gtk_widget_set_size_request(login_password_wrapper, -1, -1);
    vendor.helpers.set_classname_and_id(login_password_wrapper, "login__password_wrapper");

    GtkWidget *login_password = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(login_password, "login__password");
    gtk_widget_set_size_request(login_password, -1, 34);

    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter Password");
    gtk_box_pack_start(GTK_BOX(login_password), password_entry, TRUE, TRUE, 0);
    form_data->password_entry = password_entry;

    GtkWidget *toggle_button = gtk_toggle_button_new();
    GtkWidget *toggle_image = gtk_image_new_from_file("resources/images/static/eye.svg");
    gtk_container_add(GTK_CONTAINER(toggle_button), toggle_image);
    form_data->toggle_button = toggle_button;
    form_data->toggle_image = toggle_image;

    gtk_box_pack_start(GTK_BOX(login_password), toggle_button, FALSE, FALSE, 0);
    gtk_widget_set_can_focus(toggle_button, FALSE);
    g_signal_connect(toggle_button, "toggled", G_CALLBACK(on_toggle_password_visibility), form_data);

    gtk_box_pack_start(GTK_BOX(login_password_wrapper), login_password, FALSE, FALSE, 0);

    GtkWidget *button = gtk_button_new_with_label("Login");
    vendor.helpers.set_classname_and_id(button, "login__button");
    gtk_widget_set_size_request(button, -1, 40);
    g_signal_connect(button, "clicked", G_CALLBACK(on_login_submit), form_data);

    g_signal_connect(username_entry, "key-press-event", G_CALLBACK(on_key_press), form_data);
    g_signal_connect(password_entry, "key-press-event", G_CALLBACK(on_key_press), form_data);

    gtk_box_pack_start(GTK_BOX(login_block), login_username_wrapper, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(login_block), login_password_wrapper, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(login_block), button, TRUE, TRUE, 0);

    return login_block;
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
    };

    return login;
}
