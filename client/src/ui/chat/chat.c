#include "../../../inc/header.h"

gboolean set_scroll_to_bottom(gpointer data) {
    GtkAdjustment *vadjustment = GTK_ADJUSTMENT(data);
    gtk_adjustment_set_value(vadjustment, gtk_adjustment_get_upper(vadjustment));
    return FALSE;
}

static gboolean is_changing_chat = FALSE;

static gboolean change_chat_with_delay(gpointer data) {
    (void)data;
    is_changing_chat = FALSE;
    return G_SOURCE_REMOVE;
}

void free_chat_widgets_table(void) {
    if (vendor.pages.main_page.chat.chat_widgets_table != NULL) {
        g_hash_table_destroy(vendor.pages.main_page.chat.chat_widgets_table);
        vendor.pages.main_page.chat.chat_widgets_table = NULL;
    }
}


void change_chat(void) {
    if (is_changing_chat) {
        return;
    }

    is_changing_chat = TRUE;

    free_chat_widgets_table();

    vendor.pages.main_page.chat.page = 0;
    vendor.pages.main_page.chat.shown_messages = 0;
    vendor.pages.main_page.chat.temp_message_counter = 0;

    if (vendor.pages.main_page.chat.chat_box != NULL) {
        gtk_widget_destroy(vendor.pages.main_page.chat.chat_box);
    }

    if (vendor.active_chat.chat_sidebar_widget) {
        vendor.pages.main_page.chat.chat_box = vendor.pages.main_page.chat.init();
    } else {
        vendor.pages.main_page.chat.chat_box = vendor.pages.main_page.chat.no_chat_init();
    }

    gtk_box_pack_start(GTK_BOX(vendor.pages.main_page.main_page), vendor.pages.main_page.chat.chat_box, TRUE, TRUE, 0);
    gtk_widget_show_all(vendor.pages.current_page_widget);

    g_timeout_add(100, change_chat_with_delay, NULL);
}

GtkWidget *no_chat_init(void) {
    GtkWidget *chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(chat_box, "_right_block");
    GtkWidget *label = gtk_label_new("Select a chat to start messaging");

    gtk_widget_set_halign(label, GTK_ALIGN_FILL);
    gtk_widget_set_valign(label, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_vexpand(label, TRUE);
    gtk_box_pack_start(GTK_BOX(chat_box), label, TRUE, TRUE, 0);

    return chat_box;
}

void on_header_title_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    (void)widget;
    (void)data;

    if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
        int user_count = 0;
        t_users_struct **users = vendor.database.tables.personal_chats_table.get_users(&user_count, vendor.active_chat.chat->id);

        vendor.modal.add_users_to_group.show(GTK_WINDOW(vendor.window), user_count, users, vendor.active_chat.chat->id);
    }
}

static void my_function(const char *filename) {
    t_messages_struct *message_struct = vendor.database.tables.messages_table.add_message(vendor.database.tables.messages_table.get_total_messages() + 1,
                            vendor.active_chat.chat->id, vendor.current_user.user_id, "", filename, 0);
    ++vendor.pages.main_page.chat.temp_message_counter;
    add_chat_message(message_struct, 0);

    vendor.active_chat.chat->last_message = message_struct;

    update_chatblock(vendor.active_chat.chat_sidebar_widget, vendor.active_chat.chat, 1);
}

static void on_file_upload(GtkWidget *widget, gpointer user_data) {
    (void) widget;
    (void) user_data;
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Choose a file", NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    GtkFileFilter *filter = gtk_file_filter_new();

    gtk_file_filter_add_mime_type(filter, "image/png");
    gtk_file_filter_add_mime_type(filter, "image/jpeg");

    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

    int result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_ACCEPT) {
        char *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        time_t timestamp = time(NULL);
        struct tm *tm_info = localtime(&timestamp);
        char timestamp_str[32];
        strftime(timestamp_str, sizeof(timestamp_str), "%Y%m%d%H%M%S", tm_info);

        const char *sender_name = vendor.current_user.username;

        char new_filename[256];
        snprintf(new_filename, sizeof(new_filename), "%s_%s.jpg", sender_name, timestamp_str);

        GFile *source_file = g_file_new_for_path(file_path);
        char new_filepath[256];
        snprintf(new_filepath, sizeof(new_filepath), "resources/images/images/%s", new_filename);
        GFile *destination_file = g_file_new_for_path(new_filepath);

        GError *error = NULL;
        if (!g_file_copy(source_file, destination_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, &error)) {
            g_print("Error saving file: %s\n", error->message);
            g_error_free(error);
        } else {
            my_function(new_filename);
        }

        g_object_unref(source_file);
        g_object_unref(destination_file);
        g_free(file_path);
    }

    gtk_widget_destroy(dialog);
}

GtkWidget *chat_init(void) {
    GtkWidget *chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(chat_box, "chat");
    gtk_style_context_add_class(gtk_widget_get_style_context(chat_box), "_right_block");
    gtk_widget_set_halign(chat_box, GTK_ALIGN_FILL);
    gtk_widget_set_valign(chat_box, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(chat_box, TRUE);
    gtk_widget_set_vexpand(chat_box, TRUE);

    GtkWidget *chat_header = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(chat_header, "chat__header");
    gtk_widget_set_size_request(chat_header, -1, 65);
    gtk_box_pack_start(GTK_BOX(chat_box), chat_header, FALSE, FALSE, 0);
    gtk_widget_set_halign(chat_header, GTK_ALIGN_FILL);

    GtkWidget *header_title = gtk_label_new(vendor.active_chat.chat->name);
    vendor.helpers.set_classname_and_id(header_title, "chat__header__title");
    gtk_label_set_line_wrap(GTK_LABEL(header_title), TRUE);
    gtk_widget_set_halign(header_title, GTK_ALIGN_START);

    GtkWidget *header_title_event_box = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(header_title_event_box), header_title);
    gtk_box_pack_start(GTK_BOX(chat_header), header_title_event_box, TRUE, TRUE, 0);

    if (vendor.active_chat.chat->type == PERSONAL) {
        vendor.active_chat.recipient_public_key = vendor.database.tables.users_table.get_peer_public_key(vendor.active_chat.chat->id);
    }

    if (vendor.active_chat.chat->type == GROUP) {
        vendor.helpers.add_hover(header_title_event_box);
        g_signal_connect(header_title_event_box, "button-press-event", G_CALLBACK(on_header_title_clicked), NULL);
    }

    char temp[48];
    snprintf(temp, sizeof(temp), "id: %d", vendor.active_chat.chat->id);

    GtkWidget *header_info = gtk_label_new(temp);
    vendor.helpers.set_classname_and_id(header_info, "chat__header__info");
    gtk_label_set_line_wrap(GTK_LABEL(header_info), TRUE);
    gtk_widget_set_halign(header_info, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(chat_header), header_info, TRUE, TRUE, 0);

    GtkWidget *scrolled_window = chat_create_scrolled_window();
    gtk_box_pack_start(GTK_BOX(chat_box), scrolled_window, TRUE, TRUE, 0);

    GtkWidget *chat_box_bottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(chat_box_bottom, "chat__bottom");
    gtk_widget_set_size_request(chat_box_bottom, -1, 72);
    gtk_box_pack_start(GTK_BOX(chat_box), chat_box_bottom, FALSE, FALSE, 0);

    GtkWidget *message_send = gtk_button_new();
    GtkWidget *message_send_image = gtk_image_new_from_file("resources/images/static/plane.svg");
    vendor.helpers.set_classname_and_id(message_send, "chat__button");
    gtk_container_add(GTK_CONTAINER(message_send), message_send_image);
    vendor.helpers.add_hover(message_send);
    gtk_widget_set_size_request(message_send, 34, -1);

    GtkWidget *upload_file = gtk_button_new();
    GtkWidget *upload_file_image = gtk_image_new_from_file("resources/images/static/clip.svg");
    gtk_container_add(GTK_CONTAINER(upload_file), upload_file_image);
    vendor.helpers.add_hover(upload_file);
    gtk_widget_set_size_request(upload_file, 34, -1);
    gtk_box_pack_start(GTK_BOX(chat_box_bottom), upload_file, FALSE, TRUE, 10);
    g_signal_connect(upload_file, "clicked", G_CALLBACK(on_file_upload), NULL);

    GtkWidget *message_input = create_message_input(message_send);
    gtk_box_pack_start(GTK_BOX(chat_box_bottom), message_input, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(chat_box_bottom), message_send, FALSE, TRUE, 0);
    g_idle_add(set_scroll_to_bottom, vendor.pages.main_page.chat.vadjustment);

    return chat_box;
}

t_chat init_chat(void) {
    t_chat chat = {
        .init = chat_init,
        .no_chat_init = no_chat_init,
        .change_chat = change_chat,
        .page = 0,
        .shown_messages = 0,
        .temp_message_counter = 0,

    };
    return chat;
}
