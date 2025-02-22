#include "../../../inc/header.h"
#include <gtk/gtk.h>

static GtkWidget *message_from_server = NULL;

static void on_create_group_button_clicked(GtkButton *button, gpointer user_data) {
    (void) button;
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry));

    cJSON *json_body = cJSON_CreateObject();
    cJSON_AddStringToObject(json_body, "group_name", vendor.helpers.strdup(text));
    cJSON_AddNumberToObject(json_body, "group_picture", 0);
    cJSON_AddNumberToObject(json_body, "background", 0);

    cJSON *response = vendor.ssl_struct.send_request("POST", "/create_group_chat", json_body);
    cJSON *error = cJSON_GetObjectItem(response, "error");

    if (error->valueint == 0) {
        int chat_id = cJSON_GetObjectItem(response, "chat_id")->valueint;
        vendor.database.tables.group_chats_table.create_group(chat_id, text);
        vendor.database.tables.group_chat_members_table.add_member(chat_id, vendor.current_user.user_id, "owner");
        vendor.modal.create_group.destroy();

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
        gtk_label_set_text(GTK_LABEL(message_from_server), cJSON_GetObjectItem(response, "message")->valuestring);
    }
}

static gboolean on_message_input_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
        on_create_group_button_clicked(GTK_BUTTON(user_data), widget);
        return TRUE;
    }
    return FALSE;
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

    GtkWidget *title = gtk_label_new("Create group");
    gtk_box_pack_start(GTK_BOX(wrapper), title, FALSE, FALSE, 0);
    vendor.helpers.set_classname_and_id(title, "modal__edit_title");

    GtkWidget *message_input = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(message_input), "Group name");
    gtk_box_pack_start(GTK_BOX(wrapper), message_input, TRUE, TRUE, 20);
    vendor.helpers.set_classname_and_id(message_input, "modal__create-group_input");

    message_from_server = gtk_label_new(" ");
    gtk_box_pack_start(GTK_BOX(wrapper), message_from_server, TRUE, TRUE, 0);

    GtkWidget *button = gtk_button_new_with_label("Create group");
    vendor.helpers.add_hover(button);
    vendor.helpers.set_classname_and_id(button, "modal__edit_button");
    gtk_box_pack_start(GTK_BOX(wrapper), button, FALSE, FALSE, 20);
    g_signal_connect(button, "clicked", G_CALLBACK(on_create_group_button_clicked), message_input);
    g_signal_connect(message_input, "key-press-event", G_CALLBACK(on_message_input_key_press), button);

    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    vendor.modal.create_group.window = dialog;
}

static void destroy_modal(void) {
    if (vendor.modal.create_group.window != NULL) {
        gtk_widget_destroy(vendor.modal.create_group.window);
        vendor.modal.create_group.window = NULL;
    }
}

t_modal_create_group init_modal_create_group(void) {
    t_modal_create_group create_group = {
        .window = NULL,
        .show = show_modal,
        .destroy = destroy_modal,
    };

    return create_group;
}
