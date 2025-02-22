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
    GtkWidget *errors_widget[3];
    errors_widget[0] = vendor.pages.register_page.username_wrapper;
    errors_widget[1] = vendor.pages.register_page.password_wrapper;
    errors_widget[2] = vendor.pages.register_page.password_confirm_wrapper;

    for (int i = 0; i < 3; i++) {
        GList *children = gtk_container_get_children(GTK_CONTAINER(errors_widget[i]));

        if (gtk_style_context_has_class(gtk_widget_get_style_context(errors_widget[i]), "_form_error")) {
            gtk_style_context_remove_class(gtk_widget_get_style_context(errors_widget[i]), "_form_error");
        }
        if (g_list_length(children) != 2) {
            GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(children, 2));
            gtk_widget_destroy(target_child);
        }
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

static int db_exists(const char *name) {
    char path[512];
    snprintf(path, sizeof(path), "db/%s.db", name);
    int result = access(path, F_OK) == 0;
    if (!result) {
        vendor.database.db_name = vendor.helpers.strdup(path);
        vendor.database.create_database();
    }

    return result;
}

static void perform_request_async(GTask *task, gpointer source_object, gpointer task_data, GCancellable *cancellable) {
    (void)source_object;
    (void)task_data;
    (void)cancellable;
    (void) db_exists;

    cJSON *json_body = cJSON_CreateObject();
    cJSON_AddStringToObject(json_body, "login", vendor.helpers.strdup(vendor.current_user.username));
    cJSON_AddStringToObject(json_body, "password", vendor.helpers.strdup(vendor.current_user.password));
    cJSON_AddStringToObject(json_body, "public_key", vendor.helpers.strdup(vendor.crypto.public_key_str));

    cJSON *response = vendor.ssl_struct.send_request("POST", "/register", json_body);
    cJSON *token = cJSON_GetObjectItem(response, "token");

    gboolean success = FALSE;
    if (token != NULL) {
        vendor.current_user.jwt = vendor.helpers.strdup(token->valuestring);
        success = TRUE;
        db_exists(vendor.current_user.username);

        cJSON *user = cJSON_GetObjectItem(response, "user");

        if (user != NULL) {
            cJSON *id = cJSON_GetObjectItem(user, "id");
            if (id != NULL) {
                vendor.current_user.user_id = id->valueint;
            }
        }
    }

    cJSON_Delete(response);
    cJSON_Delete(json_body);

    g_task_return_boolean(task, success);
}

static void on_request_complete(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    (void)source_object;
    (void)user_data;

    GTask *task = G_TASK(res);
    gboolean success = g_task_propagate_boolean(task, NULL);

    if (success) {
        t_active_users_struct active_users_struct = {
            .user_id = vendor.current_user.user_id,
            .username = vendor.current_user.username,
            .user_login = vendor.current_user.username,
            .about = NULL,
            .public_key = vendor.crypto.public_key_str,
            .private_key = vendor.crypto.encrypt_text(vendor.crypto.private_key_str, vendor.current_user.password),
        };

        t_users_struct users_struct = {
            .user_id = vendor.current_user.user_id,
            .username = vendor.current_user.username,
            .user_login = vendor.current_user.username,
            .about = NULL,
            .public_key = vendor.crypto.public_key_str,
        };

        vendor.database.tables.active_users_table.add_user(&active_users_struct);
        vendor.database.tables.users_table.add_user(&users_struct);
        vendor.pages.change_page(MAIN_PAGE);

    } else {
        vendor.pages.change_page(REGISTER_PAGE);
        create_error(vendor.pages.register_page.password_confirm_wrapper, "A user with this name already exists");
        gtk_style_context_add_class(gtk_widget_get_style_context(vendor.pages.register_page.password_wrapper), "_form_error");
        gtk_style_context_add_class(gtk_widget_get_style_context(vendor.pages.register_page.username_wrapper), "_form_error");
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

    vendor.current_user.username = vendor.helpers.strdup(username);
    vendor.current_user.user_login = vendor.helpers.strdup(username);
    vendor.current_user.password = vendor.helpers.strdup(password);

    vendor.pages.change_page(LOADING_PAGE);
    vendor.crypto.keygen();

    GTask *task = g_task_new(NULL, NULL, on_request_complete, NULL);
    g_task_run_in_thread(task, perform_request_async);
    g_object_unref(task);
}

gboolean register_input_on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    (void)widget;
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
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
