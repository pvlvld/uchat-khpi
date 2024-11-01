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

    gtk_style_context_add_class(gtk_widget_get_style_context(widget), "_form_error");

    vendor.helpers.set_classname_and_id(error_wrapper, "_form__error_wrapper");
    vendor.helpers.set_classname_and_id(error, "_form__error_text");

    gtk_widget_show_all(error_wrapper);
}

static gboolean validate_input(const gchar *username, const gchar *password) {
    gboolean is_valid = TRUE;

    GList *usrname_children = gtk_container_get_children(GTK_CONTAINER(vendor.pages.login_page.username_wrapper));
    if (g_list_length(usrname_children) != 2) {
        GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(usrname_children, 2));
        gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.pages.login_page.username_wrapper), "_form_error");
        gtk_widget_destroy(target_child);
    }

    GList *password_children = gtk_container_get_children(GTK_CONTAINER(vendor.pages.login_page.password_wrapper));
    if (g_list_length(password_children) != 2) {
        GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(password_children, 2));
        gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.pages.login_page.password_wrapper), "_form_error");
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


// Show placeholder when entry gains focus
static gboolean on_from_entry_focus_in(GtkWidget *entry, GdkEventFocus *event, gpointer user_data) {
    (void)entry;
    (void)event;
    GtkWidget *placeholder = (GtkWidget *)user_data;
    gtk_widget_hide(placeholder);
    return FALSE;
}

// Show placeholder when entry loses focus and is empty
static gboolean on_form_entry_focus_out(GtkWidget *entry, GdkEventFocus *event, gpointer user_data) {
    (void)event;
    GtkWidget *placeholder = (GtkWidget *)user_data;
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (g_strcmp0(text, "") == 0) {
        gtk_widget_show(placeholder);
    }
    return FALSE;
}

GtkWidget *create_login_button(void) {
    GtkWidget *button = gtk_button_new_with_label("Login");
    vendor.helpers.set_classname_and_id(button, "login__button");
    gtk_widget_set_size_request(button, -1, 40);
    g_signal_connect(button, "clicked", G_CALLBACK(on_login_submit), &vendor.pages.login_page.form_data);

    return button;
}

GtkWidget *create_login_username_wrapper(void) {
    t_login_form_data *form_data = &vendor.pages.login_page.form_data;

    GtkWidget *login_username_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.pages.login_page.username_wrapper = login_username_wrapper;
    gtk_widget_set_size_request(login_username_wrapper, -1, -1);
    vendor.helpers.set_classname_and_id(login_username_wrapper, "login__username_wrapper");
    gtk_style_context_add_class(gtk_widget_get_style_context(login_username_wrapper), "_form__input_wrapper");

    GtkWidget *login_username = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(login_username, "login__username");
    gtk_style_context_add_class(gtk_widget_get_style_context(login_username), "_form__input_block");
    gtk_widget_set_size_request(login_username, -1, 34);
    gtk_widget_set_halign(login_username, GTK_ALIGN_FILL);

    GtkWidget *login_username_image_wrapper = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(login_username_image_wrapper, 32, -1);
    vendor.helpers.set_classname_and_id(login_username_image_wrapper, "login__username_image__wrapper");
	gtk_style_context_add_class(gtk_widget_get_style_context(login_username_image_wrapper), "_form__image_wrapper");
	gtk_box_pack_start(GTK_BOX(login_username), login_username_image_wrapper, FALSE, FALSE, 0);

    GtkWidget *login_username_image = gtk_image_new_from_file("resources/images/static/username.svg");
    gtk_widget_set_size_request(login_username_image, 12, 12);
    gtk_box_pack_start(GTK_BOX(login_username_image_wrapper), login_username_image, TRUE, TRUE, 0);

    GtkWidget *username_entry = gtk_entry_new();
    vendor.helpers.set_classname_and_id(username_entry, "login__username_input");
    gtk_style_context_add_class(gtk_widget_get_style_context(username_entry), "_form__input");
    gtk_box_pack_start(GTK_BOX(login_username), username_entry, TRUE, TRUE, 0);
    form_data->username_entry = username_entry;

    GtkWidget *username_placeholder = gtk_label_new("Enter Username");
    gtk_style_context_add_class(gtk_widget_get_style_context(username_placeholder), "_form__input_placeholder");
    gtk_widget_set_halign(username_placeholder, GTK_ALIGN_START);

	gtk_box_pack_start(GTK_BOX(login_username_wrapper), login_username, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(login_username_wrapper), username_placeholder, TRUE, FALSE, 0);

    g_signal_connect(username_entry, "focus-in-event", G_CALLBACK(on_from_entry_focus_in), username_placeholder);
    g_signal_connect(username_entry, "focus-out-event", G_CALLBACK(on_form_entry_focus_out), username_placeholder);
    gtk_widget_show(username_placeholder);

    g_signal_connect(username_entry, "key-press-event", G_CALLBACK(on_key_press), form_data);

    return login_username_wrapper;
}

GtkWidget *create_login_password_wrapper(void) {
    t_login_form_data *form_data = &vendor.pages.login_page.form_data;

GtkWidget *login_password_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.pages.login_page.password_wrapper = login_password_wrapper;
    gtk_widget_set_size_request(login_password_wrapper, -1, -1);
    vendor.helpers.set_classname_and_id(login_password_wrapper, "login__password_wrapper");
    gtk_style_context_add_class(gtk_widget_get_style_context(login_password_wrapper), "_form__input_wrapper");

    GtkWidget *login_password = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(login_password, "login__password");
    gtk_style_context_add_class(gtk_widget_get_style_context(login_password), "_form__input_block");
    gtk_widget_set_size_request(login_password, -1, 34);

    GtkWidget *login_password_image_wrapper = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(login_password_image_wrapper, 32, -1);
    vendor.helpers.set_classname_and_id(login_password_image_wrapper, "login__passowrd_image__wrapper");
	gtk_style_context_add_class(gtk_widget_get_style_context(login_password_image_wrapper), "_form__image_wrapper");
	gtk_box_pack_start(GTK_BOX(login_password), login_password_image_wrapper, FALSE, FALSE, 0);

    GtkWidget *login_password_image = gtk_image_new_from_file("resources/images/static/password.svg");
    gtk_widget_set_size_request(login_password_image, 12, 15);
    gtk_box_pack_start(GTK_BOX(login_password_image_wrapper), login_password_image, TRUE, TRUE, 0);

    GtkWidget *password_entry = gtk_entry_new();
    vendor.helpers.set_classname_and_id(password_entry, "login__password_input");
    gtk_style_context_add_class(gtk_widget_get_style_context(password_entry), "_form__input");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_box_pack_start(GTK_BOX(login_password), password_entry, TRUE, TRUE, 0);
    form_data->password_entry = password_entry;

    GtkWidget *password_placeholder = gtk_label_new("Enter Password");
    gtk_style_context_add_class(gtk_widget_get_style_context(password_placeholder), "_form__input_placeholder");
    gtk_widget_set_halign(password_placeholder, GTK_ALIGN_START);

	gtk_box_pack_start(GTK_BOX(login_password_wrapper), login_password, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(login_password_wrapper), password_placeholder, TRUE, FALSE, 0);

    g_signal_connect(password_entry, "focus-in-event", G_CALLBACK(on_from_entry_focus_in), password_placeholder);
    g_signal_connect(password_entry, "focus-out-event", G_CALLBACK(on_form_entry_focus_out), password_placeholder);
    gtk_widget_show(password_placeholder);

    GtkWidget *toggle_button = gtk_toggle_button_new();
    GtkWidget *toggle_image = gtk_image_new_from_file("resources/images/static/eye.svg");
    gtk_container_add(GTK_CONTAINER(toggle_button), toggle_image);
    form_data->toggle_button = toggle_button;
    form_data->toggle_image = toggle_image;

    gtk_box_pack_start(GTK_BOX(login_password), toggle_button, FALSE, FALSE, 0);
    gtk_widget_set_can_focus(toggle_button, FALSE);
    g_signal_connect(toggle_button, "toggled", G_CALLBACK(on_toggle_password_visibility), form_data);
    gtk_style_context_add_class(gtk_widget_get_style_context(toggle_button), "_form__input_toggle");
    g_signal_connect(password_entry, "key-press-event", G_CALLBACK(on_key_press), form_data);

    return login_password_wrapper;
}

GtkWidget *login_init(void) {
    GtkWidget *login_block = gtk_box_new(GTK_ORIENTATION_VERTICAL, 24);
    vendor.helpers.set_classname_and_id(login_block, "login__block");
    gtk_widget_set_halign(login_block, GTK_ALIGN_FILL);
    gtk_widget_set_size_request(login_block, 308, 155);
    gtk_widget_set_hexpand(login_block, FALSE);

    GtkWidget *login_username_wrapper = create_login_username_wrapper();

    GtkWidget *login_password_wrapper = create_login_password_wrapper();

    GtkWidget *button = create_login_button();

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
