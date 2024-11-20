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

static void remove_errors(void) {
	GList *usrname_children = gtk_container_get_children(GTK_CONTAINER(vendor.pages.register_page.username_wrapper));
    if (g_list_length(usrname_children) != 2) {
        GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(usrname_children, 2));
        gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.pages.register_page.username_wrapper), "_form_error");
        gtk_widget_destroy(target_child);
    }

    GList *password_children = gtk_container_get_children(GTK_CONTAINER(vendor.pages.register_page.password_wrapper));
    if (g_list_length(password_children) != 2) {
        GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(password_children, 2));
        gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.pages.register_page.password_wrapper), "_form_error");
        gtk_widget_destroy(target_child);
    }

    GList *password_confim_children = gtk_container_get_children(GTK_CONTAINER(vendor.pages.register_page.password_confirm_wrapper));
    if (g_list_length(password_confim_children) != 2) {
        GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(password_confim_children, 2));
        gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.pages.register_page.password_confirm_wrapper), "_form_error");
        gtk_widget_destroy(target_child);
    }
}

static gboolean validate_input(const gchar *username, const gchar *password, const gchar *password_confirm) {
    gboolean is_valid = TRUE;

    remove_errors();

    if (g_strcmp0(username, "") == 0 || g_utf8_strlen(username, -1) < 3) {
        create_error(vendor.pages.register_page.username_wrapper, "Username must be at least 3 characters long");
        is_valid = FALSE;
    } else if (!g_regex_match_simple("^[a-zA-Z0-9_]+$", username, 0, 0)) {
        create_error(vendor.pages.register_page.username_wrapper, "Username can only contain letters, digits, and underscores");
        is_valid = FALSE;
    }

    if (g_strcmp0(password, "") == 0 || strlen(password) < 8) {
        create_error(vendor.pages.register_page.password_wrapper, "Password must be at least 8 characters long");
        is_valid = FALSE;
    } else if (!g_regex_match_simple(".*[A-Za-z].*", password, 0, 0)) {
        create_error(vendor.pages.register_page.password_wrapper, "Password must contain at least one letter");
        is_valid = FALSE;
    } else if (!g_regex_match_simple(".*[0-9].*", password, 0, 0)) {
        create_error(vendor.pages.register_page.password_wrapper, "Password must contain at least one digit");
        is_valid = FALSE;
    } else if (!g_regex_match_simple(".*[@#$%^&*()/\\-+=!<>?].*", password, 0, 0)) {
        create_error(vendor.pages.register_page.password_wrapper, "Password must contain at least one special character (e.g., @, #, $, %, &)");
        is_valid = FALSE;
    }

     if (g_strcmp0(password, password_confirm) != 0) {
          create_error(vendor.pages.register_page.password_confirm_wrapper, "The passwords don't match");
          is_valid = FALSE;
     }

    return is_valid;
}


static void perform_request_async(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable) {
    (void)source_object;
    (void)task_data;
    (void)cancellable;

    g_usleep(300000);

//    t_active_users_struct active_users_struct = {
//        .user_id = 1,
//        .username = vendor.current_user.username,
//        .user_login = vendor.current_user.username,
//        .about = NULL,
//        .public_key = vendor.crypto.public_key_str,
//        .private_key = vendor.crypto.encrypt_text(vendor.crypto.private_key_str, vendor.current_user.password),
//    };
//    vendor.database.tables.active_users_table.add_user(&active_users_struct);
//    printf("User id: %d\n", active_users_struct.user_id);

    g_task_return_boolean(task, TRUE);
}

static void on_request_complete(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    (void)source_object;
    (void)user_data;

    GTask *task = G_TASK(res);
    gboolean success = g_task_propagate_boolean(task, NULL);

    if (success) {
        t_active_users_struct active_users_struct = {
            .user_id = 1,
            .username = vendor.current_user.username,
            .user_login = vendor.current_user.username,
            .about = NULL,
            .public_key = vendor.crypto.public_key_str,
            .private_key = vendor.crypto.encrypt_text(vendor.crypto.private_key_str, vendor.current_user.password),
        };
        vendor.database.tables.active_users_table.add_user(&active_users_struct);
        vendor.pages.change_page(MAIN_PAGE);

    } else {
        g_print("Request failed.\n");
    }
}

void register_on_register_submit(GtkButton *button, gpointer user_data) {
    (void)button;
    t_register_form_data *data = (t_register_form_data *)user_data;

    const gchar *username = gtk_entry_get_text(GTK_ENTRY(data->username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(data->password_entry));
    const gchar *password_confirm = gtk_entry_get_text(GTK_ENTRY(data->password_confirm_entry));

    gboolean is_valid = validate_input(username, password, password_confirm);

    if (!is_valid) {
        g_print("Invalid input detected.\n");
        return;
    }

    g_print("login: %s\nPassword: %s\n", username, password);
    vendor.current_user.username = vendor.helpers.strdup(username);
    vendor.current_user.password = vendor.helpers.strdup(password);

    vendor.pages.change_page(LOADING_PAGE);
    vendor.crypto.keygen();

    GTask *task = g_task_new(NULL, NULL, on_request_complete, NULL);
    g_task_run_in_thread(task, perform_request_async);
    g_object_unref(task);
}

gboolean register_input_on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    (void)widget;
    if (event->keyval == GDK_KEY_Return) {
        register_on_register_submit(NULL, user_data);
        return TRUE;
    }
    return FALSE;
}

// Show placeholder when entry gains focus
gboolean register_on_from_entry_focus_in(GtkWidget *entry, GdkEventFocus *event, gpointer user_data) {
    (void)entry;
    (void)event;
    GtkWidget *placeholder = (GtkWidget *)user_data;
    gtk_widget_hide(placeholder);
    remove_errors();
    return FALSE;
}

// Show placeholder when entry loses focus and is empty
gboolean register_on_form_entry_focus_out(GtkWidget *entry, GdkEventFocus *event, gpointer user_data) {
    (void)event;
    GtkWidget *placeholder = (GtkWidget *)user_data;
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (g_strcmp0(text, "") == 0) {
        gtk_widget_show(placeholder);
    }
    return FALSE;
}

t_register_functions init_register_functions(void) {
    t_register_functions register_functions = {
        .on_key_press = register_input_on_key_press,
        .on_from_entry_focus_in = register_on_from_entry_focus_in,
        .on_from_entry_focus_out = register_on_form_entry_focus_out,
        .on_register_submit = register_on_register_submit,
        .create_username_wrapper = create_register_username_wrapper,
        .create_password_wrapper = create_register_password_wrapper,
        .create_password_confirm_wrapper = create_register_password_confirm_wrapper,
        .create_button = create_register_button,
    };
    return register_functions;
}
