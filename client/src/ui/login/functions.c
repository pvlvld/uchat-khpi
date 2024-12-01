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
    GList *password_children = gtk_container_get_children(GTK_CONTAINER(vendor.pages.login_page.password_wrapper));
    if (g_list_length(password_children) != 2) {
        GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(password_children, 2));
        gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.pages.login_page.password_wrapper), "_form_error");
        gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.pages.login_page.username_wrapper), "_form_error");
        gtk_widget_destroy(target_child);
    }
}

static gboolean validate_input(const gchar *username, const gchar *password) {
    gboolean is_valid = TRUE;

    remove_errors();

    if (g_strcmp0(username, "") == 0 || g_utf8_strlen(username, -1) < 3
            || !g_regex_match_simple("^[a-zA-Z0-9_]+$", username, 0, 0)
            || g_strcmp0(password, "") == 0 || strlen(password) < 8
            || !g_regex_match_simple(".*[A-Za-z].*", password, 0, 0)
            || !g_regex_match_simple(".*[0-9].*", password, 0, 0)
            || !g_regex_match_simple(".*[@#$%^&*()/\\-+=!<>?].*", password, 0, 0)) {
        create_error(vendor.pages.login_page.password_wrapper, "Incorrect username or password");
        gtk_style_context_add_class(gtk_widget_get_style_context(vendor.pages.login_page.username_wrapper), "_form_error");
        is_valid = FALSE;
    }

    return is_valid;
}


static void perform_request_async(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable) {
    (void)source_object;
    (void)task_data;
    (void)cancellable;

    g_usleep(300000); // 0.3s

    g_task_return_boolean(task, TRUE);
}

static void on_request_complete(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    (void)source_object;
    (void)user_data;

    GTask *task = G_TASK(res);
    gboolean success = g_task_propagate_boolean(task, NULL);

    t_active_users_struct *active_users_struct = vendor.database.tables.active_users_table.get_user_by_user_login(vendor.current_user.username);
    vendor.crypto.public_key_str = active_users_struct->public_key;
    vendor.crypto.private_key_str = vendor.crypto.decrypt_text(active_users_struct->private_key, vendor.current_user.password);

    if (!active_users_struct) {
        vendor.popup.add_message("Unexpected error");
        vendor.pages.change_page(LOGIN_PAGE);
        return;
    }

    if (success && vendor.crypto.verify_key_pair()) {
        vendor.current_user.user_id = active_users_struct->user_id;
        vendor.current_user.username = active_users_struct->username;
        vendor.current_user.user_login = active_users_struct->user_login;
        vendor.current_user.about = active_users_struct->about;

//        char *encrypt = vendor.crypto.encrypt(active_users_struct->public_key, "Test message");
//        if (encrypt) {
//            char *decrypt = vendor.crypto.decrypt(encrypt);
//            if (decrypt) {
//                printf("%s\n", decrypt);
//                free(decrypt);
//            }
//            free(encrypt);
//        }
        vendor.pages.change_page(MAIN_PAGE);
    } else {
        vendor.pages.change_page(LOGIN_PAGE);
        create_error(vendor.pages.login_page.password_wrapper, "Incorrect username or password");
        gtk_style_context_add_class(gtk_widget_get_style_context(vendor.pages.login_page.username_wrapper), "_form_error");
    }
}

static int db_exists(const char *name) {
    char path[512];
    snprintf(path, sizeof(path), "db/%s.db", name);
    int result = access(path, F_OK) == 0;
    if (result) {
        vendor.database.db_name = vendor.helpers.strdup(path);
        vendor.database.create_database();
    }

    return result;
}

void login_on_login_submit(GtkButton *button, gpointer user_data) {
    (void)button;
    t_login_form_data *data = (t_login_form_data *)user_data;

    const gchar *username = gtk_entry_get_text(GTK_ENTRY(data->username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(data->password_entry));

    gboolean is_valid = validate_input(username, password);

    if (!is_valid) {
        g_print("Invalid input detected.\n");
        return;
    }

    if (db_exists(username)) {
        vendor.current_user.username = vendor.helpers.strdup(username);
        vendor.current_user.password = vendor.helpers.strdup(password);
        vendor.pages.change_page(LOADING_PAGE);

        GTask *task = g_task_new(NULL, NULL, on_request_complete, NULL);
        g_task_run_in_thread(task, perform_request_async);
        g_object_unref(task);
    } else {
        create_error(vendor.pages.login_page.password_wrapper, "This user does not have a key on this device.");
        gtk_style_context_add_class(gtk_widget_get_style_context(vendor.pages.login_page.username_wrapper), "_form_error");
    }

}

gboolean login_input_on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    (void)widget;
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
        login_on_login_submit(NULL, user_data);
        return TRUE;
    }
    return FALSE;
}

// Show placeholder when entry gains focus
gboolean login_on_from_entry_focus_in(GtkWidget *entry, GdkEventFocus *event, gpointer user_data) {
    (void)entry;
    (void)event;
    GtkWidget *placeholder = (GtkWidget *)user_data;
    gtk_widget_hide(placeholder);
    remove_errors();
    return FALSE;
}

// Show placeholder when entry loses focus and is empty
gboolean login_on_form_entry_focus_out(GtkWidget *entry, GdkEventFocus *event, gpointer user_data) {
    (void)event;
    GtkWidget *placeholder = (GtkWidget *)user_data;
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    if (g_strcmp0(text, "") == 0) {
        gtk_widget_show(placeholder);
    }
    return FALSE;
}

t_login_functions init_login_functions(void) {
    t_login_functions login_functions = {
        .on_key_press = login_input_on_key_press,
        .on_from_entry_focus_in = login_on_from_entry_focus_in,
        .on_from_entry_focus_out = login_on_form_entry_focus_out,
        .on_login_submit = login_on_login_submit,
        .create_username_wrapper = create_login_username_wrapper,
        .create_password_wrapper = create_login_password_wrapper,
        .create_button = create_login_button,
    };
    return login_functions;
}
