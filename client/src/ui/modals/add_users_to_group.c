#include "../../../inc/header.h"
#include <gtk/gtk.h>

static GtkWidget *message_from_server = NULL;

void on_add_to_group_clicked(GtkButton *button, t_user_block_data *data) {
    (void) button;

    char chat_id_str[20];
    sprintf(chat_id_str, "%d", data->chat_id);

    cJSON *json_body = cJSON_CreateObject();
    cJSON_AddStringToObject(json_body, "chat_id", vendor.helpers.strdup(chat_id_str));
    cJSON_AddStringToObject(json_body, "username", data->username);

    cJSON *response = vendor.ssl_struct.send_request("POST", "/add_user_to_group", json_body);
    cJSON *error = cJSON_GetObjectItem(response, "error");

    if (error->valueint == 0) {
        gtk_widget_destroy(data->userblock);
        vendor.database.tables.group_chat_members_table.add_member(data->chat_id, data->user_id, "member");
    } else {
        gtk_label_set_text(GTK_LABEL(message_from_server), cJSON_GetObjectItem(response, "message")->valuestring);
    }


    g_free(data);
}

static void show_modal(GtkWindow *parent, int user_count, t_users_struct **users, int chat_id) {
    const int WIDTH = 500;
    const int HEIGHT = 400;
    GtkWidget *dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Add users to group");
    vendor.helpers.set_classname_and_id(dialog, "modal__edit");
    gtk_style_context_add_class(gtk_widget_get_style_context(dialog), "_modal-base");
    gtk_widget_set_visual(dialog, gdk_screen_get_rgba_visual(gtk_widget_get_screen(dialog)));

    gtk_window_set_default_size(GTK_WINDOW(dialog), WIDTH, HEIGHT);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER_ON_PARENT);

    GtkWidget *wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(wrapper, WIDTH, HEIGHT);
    gtk_container_add(GTK_CONTAINER(dialog), wrapper);
    vendor.helpers.set_classname_and_id(wrapper, "modal__edit_wrapper");
    gtk_widget_set_halign(wrapper, GTK_ALIGN_START);

    GtkWidget *title = gtk_label_new("Add users to group");
    vendor.helpers.set_classname_and_id(title, "modal__edit_title");

    GtkWidget *title_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(title_wrapper), title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(wrapper), title_wrapper, FALSE, FALSE, 0);

    message_from_server = gtk_label_new(" ");
    gtk_box_pack_start(GTK_BOX(title_wrapper), message_from_server, TRUE, TRUE, 0);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(wrapper), scrolled_window, TRUE, TRUE, 20);

    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(scrolled_window), content_box);
    gtk_widget_set_valign(content_box, GTK_ALIGN_START);

    for (int i = 0; i < user_count; i++) {
        GtkWidget *userblock = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

        GtkWidget *avatar = vendor.helpers.create_avatar(users[i]->username, 48, 48);
        gtk_box_pack_start(GTK_BOX(userblock), avatar, FALSE, FALSE, 0);

        GtkWidget *label = gtk_label_new(users[i]->username);
        gtk_box_pack_start(GTK_BOX(userblock), label, FALSE, FALSE, 0);

        GtkWidget *button = gtk_button_new_with_label("Add to group");
        gtk_box_pack_end(GTK_BOX(userblock), button, FALSE, FALSE, 0);
        vendor.helpers.add_hover(button);
        vendor.helpers.set_classname_and_id(button, "modal__add_to_group");

        t_user_block_data *data = g_new(t_user_block_data, 1);
        data->userblock = userblock;
        data->user_id = users[i]->user_id;
        data->chat_id = chat_id;
        data->username = users[i]->username;

        g_signal_connect(button, "clicked", G_CALLBACK(on_add_to_group_clicked), data);

        gtk_box_pack_start(GTK_BOX(content_box), userblock, FALSE, FALSE, 0);
        gtk_widget_show_all(userblock);
    }
    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    vendor.modal.add_users_to_group.window = dialog;
}


static void destroy_modal(void) {
    if (vendor.modal.add_users_to_group.window != NULL) {
        gtk_widget_destroy(vendor.modal.add_users_to_group.window);
        vendor.modal.add_users_to_group.window = NULL;
    }
}

t_modal_add_users_to_group init_modal_add_users_to_group(void) {
    t_modal_add_users_to_group add_users_to_group = {
        .window = NULL,
        .show = show_modal,
        .destroy = destroy_modal,
    };

    return add_users_to_group;
}
