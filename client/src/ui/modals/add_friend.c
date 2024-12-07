#include "../../../inc/header.h"
#include <gtk/gtk.h>

static void on_add_friend_button_clicked(GtkButton *button, gpointer user_data) {
    (void) button;
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *username = gtk_entry_get_text(GTK_ENTRY(entry));

    cJSON *json_body = cJSON_CreateObject();
    cJSON_AddStringToObject(json_body, "username", vendor.helpers.strdup(username));

    cJSON *response = vendor.ssl_struct.send_request("POST", "/friend_request", json_body);
    cJSON *error = cJSON_GetObjectItem(response, "error");
    if (error->valueint == 0) {
        t_users_struct *friend_struct = malloc(sizeof(t_users_struct));
        friend_struct->user_id = cJSON_GetObjectItem(response, "recipient_id")->valueint;
        friend_struct->username = cJSON_GetObjectItem(response, "recipient_login")->valuestring;
        friend_struct->user_login = cJSON_GetObjectItem(response, "recipient_login")->valuestring;
        friend_struct->about = cJSON_GetObjectItem(response, "recipient_about")->valuestring;
        friend_struct->public_key = cJSON_GetObjectItem(response, "recipient_public_key")->valuestring;
        friend_struct->public_key = cJSON_GetObjectItem(response, "recipient_public_key")->valuestring;

        int chat_id = cJSON_GetObjectItem(response, "chat_id")->valueint;
        const char *timestamp_str = cJSON_GetObjectItem(response, "timestamp")->valuestring;

        if (timestamp_str != NULL) {
            struct tm tm;
            if (sscanf(timestamp_str, "%4d-%2d-%2d %2d:%2d:%2d",
                &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
                &tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6) {
                tm.tm_year -= 1900;
                tm.tm_mon -= 1;
                time_t timestamp = mktime(&tm);
                friend_struct->updated_at = *localtime(&timestamp);
            } else {
                printf("[ERROR] Invalid timestamp format\n");
            }
        } else {
            printf("[ERROR] Timestamp is NULL\n");
        }


        vendor.database.tables.users_table.add_user(friend_struct);

        vendor.database.tables.chats_table.add_chat(chat_id, "personal");
        vendor.database.tables.personal_chats_table.create_personal_chat(chat_id, friend_struct->user_id);

        t_chat_info *chat_info = parse_chat_info_by_id(chat_id);
        GtkWidget *chatblock = vendor.pages.main_page.sidebar.create_chatblock(chat_info);
        if (!chatblock) {
            printf("[ERROR] Failed to create a block for chat with ID: %d\n", chat_info->id);
            return;
        }

        g_object_set_data(G_OBJECT(chatblock), "chat_info", chat_info);
        gtk_box_pack_start(GTK_BOX(vendor.sidebar.stretchable_box), chatblock, FALSE, FALSE, 0);
        gtk_box_reorder_child(GTK_BOX(vendor.sidebar.stretchable_box), chatblock, 0);
        gtk_widget_show_all(vendor.sidebar.stretchable_box);
    } else {
        g_print("[ERROR]\n");
        g_print("%s\n", cJSON_Print(response));
    }
}

static void show_modal(GtkWindow *parent) {
    const int WIDTH = 500;
    const int HEIGHT = 200;
    GtkWidget *dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Create group");
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

    GtkWidget *title = gtk_label_new("Add friend");
    gtk_box_pack_start(GTK_BOX(wrapper), title, FALSE, FALSE, 0);
    vendor.helpers.set_classname_and_id(title, "modal__edit_title");

    GtkWidget *message_input = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(message_input), "Friend username");
    gtk_box_pack_start(GTK_BOX(wrapper), message_input, TRUE, TRUE, 20);
    vendor.helpers.set_classname_and_id(message_input, "modal__create-group_input");

    GtkWidget *button = gtk_button_new_with_label("Find friend");
    vendor.helpers.add_hover(button);
    vendor.helpers.set_classname_and_id(button, "modal__edit_button");
    gtk_box_pack_start(GTK_BOX(wrapper), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_add_friend_button_clicked), message_input);

    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    vendor.modal.add_friend.window = dialog;
}

static void destroy_modal(void) {
    if (vendor.modal.add_friend.window != NULL) {
        gtk_widget_destroy(vendor.modal.add_friend.window);
        vendor.modal.add_friend.window = NULL;
    }
}

t_modal_add_friend init_modal_add_friend(void) {
    t_modal_add_friend add_friend = {
        .window = NULL,
        .show = show_modal,
        .destroy = destroy_modal,
    };

    return add_friend;
}
