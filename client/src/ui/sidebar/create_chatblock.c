#include "../../../inc/header.h"
#include <gtk/gtk.h>

static char *format_last_message(const char *sender_name, const char *message, int chat_type) {
    if (chat_type == 0 || sender_name == NULL || strlen(sender_name) == 0) {
        return g_strdup(message);
    } else {
        // Групповой чат с именем отправителя
        const char *format = "<span foreground='#047857'><b>%s: </b></span>%s";
        int size = snprintf(NULL, 0, format, sender_name, message);
        if (size < 0) {
            return NULL;
        }

        char *buffer = (char *)malloc(size + 1);
        if (!buffer) {
            return NULL;
        }

        snprintf(buffer, size + 1, format, sender_name, message);
        return buffer;
    }
}


static gboolean click_handler(GtkWidget *widget, GdkEventButton *event) {
    (void) event;
    if (event->button == GDK_BUTTON_PRIMARY) {
        t_chat_info *chat_info = g_object_get_data(G_OBJECT(widget), "chat_info");
        if (chat_info != NULL) {
            vendor.active_chat.chat = chat_info;
        }
        if (vendor.active_chat.chat_sidebar_widget != NULL) {
            gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.active_chat.chat_sidebar_widget), "active");
        }
        vendor.active_chat.chat_sidebar_widget = widget;
        vendor.pages.main_page.chat.change_chat();
        gtk_style_context_add_class(gtk_widget_get_style_context(vendor.active_chat.chat_sidebar_widget), "active");
    } else if (event->button == GDK_BUTTON_SECONDARY) {
        int x = event->x_root;
        int y = event->y_root;
        if (vendor.active_chat.chat_sidebar_widget != widget) {
            t_chat_info *chat_info = g_object_get_data(G_OBJECT(widget), "chat_info");

            if (chat_info != NULL) {
                vendor.hover_chat.chat = chat_info;
            }
            if (vendor.hover_chat.chat_sidebar_widget != NULL) {
                gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.hover_chat.chat_sidebar_widget), "hover");
            }
            vendor.hover_chat.chat_sidebar_widget = widget;
            gtk_style_context_add_class(gtk_widget_get_style_context(vendor.hover_chat.chat_sidebar_widget), "hover");
        }

        vendor.modal.chat_info.show(GTK_WINDOW(gtk_widget_get_toplevel(widget)), x, y);
    }

    return TRUE;
}

static char *replace_newlines_with_spaces(const char *input) {
    size_t len = strlen(input);
    size_t new_len = len;

    for (size_t i = 0; i < len; i++) {
        if (input[i] == '\n') {
            new_len += 2;
        }
    }

    char *result = (char *)malloc(new_len + 1);
    if (result == NULL) {
        return NULL;
    }

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (input[i] == '\n') {
            result[j++] = ' ';
            result[j++] = ' ';
            result[j++] = ' ';
        } else {
            result[j++] = input[i];
        }
    }

    result[j] = '\0';
    return result;
}

static GtkWidget *create_chat_name(t_chat_info *chat_info) {
    GtkWidget *chat_name = gtk_label_new(chat_info->name);
    vendor.helpers.set_classname_and_id(chat_name, "sidebar__chatblock_chat__name");
    gtk_label_set_lines(GTK_LABEL(chat_name), 1);
    gtk_label_set_ellipsize(GTK_LABEL(chat_name), PANGO_ELLIPSIZE_END);

    return chat_name;
}

static char *create_formatted_last_message(t_chat_info *chat_info) {
    char *stripped_last_message = NULL;
    char *stripped_sender_name = NULL;
    char *formatted_last_message = NULL;

    if (chat_info->last_message && chat_info->last_message->sender_struct->username) {
        char *last_message_copy = g_strdup(chat_info->last_message->message_text ? chat_info->last_message->message_text : "");
        char *sender_name_copy = g_strdup(chat_info->last_message->sender_struct->username ? chat_info->last_message->sender_struct->username : "");

        stripped_last_message = g_strstrip(last_message_copy);
        stripped_sender_name = g_strstrip(sender_name_copy);
        if (chat_info->type == PERSONAL) {
            formatted_last_message = g_strdup(stripped_last_message);
        } else {  // group
            formatted_last_message = format_last_message(stripped_sender_name, stripped_last_message, chat_info->type);
        }
        g_free(last_message_copy);
        g_free(sender_name_copy);
    } else {
        if (chat_info->type == PERSONAL) {
            formatted_last_message = g_strdup(stripped_last_message);
        }
        formatted_last_message = "No messages";
    }

    return replace_newlines_with_spaces(formatted_last_message ? formatted_last_message : "");
}

static GtkWidget *create_chat_message(t_chat_info *chat_info) {
    GtkWidget *chat_message = gtk_label_new(create_formatted_last_message(chat_info));

    gtk_label_set_use_markup(GTK_LABEL(chat_message), TRUE);
    gtk_label_set_line_wrap(GTK_LABEL(chat_message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(chat_message), 0.0);
    gtk_label_set_lines(GTK_LABEL(chat_message), 2);
    gtk_label_set_ellipsize(GTK_LABEL(chat_message), PANGO_ELLIPSIZE_END);
    vendor.helpers.set_classname_and_id(chat_message, "sidebar__chatblock_chat__message");
    return chat_message;
}

static char *create_chat_time_string(t_chat_info *chat_info) {
    return format_timestamp(chat_info->last_message ? chat_info->last_message->timestamp : *localtime(&chat_info->timestamp));
}

static GtkWidget *create_chat_time(t_chat_info *chat_info) {
    GtkWidget *chat_time = gtk_label_new(create_chat_time_string(chat_info));
    vendor.helpers.set_classname_and_id(chat_time, "sidebar__chatblock_chat__time");

    return chat_time;
}

static void update_chat_message(GtkWidget *chatblock, const char *new_message) {
    GtkWidget *chat_message = g_object_get_data(G_OBJECT(chatblock), "chat_message");

    if (chat_message != NULL) {
        gtk_label_set_text(GTK_LABEL(chat_message), new_message);
        gtk_label_set_use_markup(GTK_LABEL(chat_message), TRUE);
        gtk_label_set_line_wrap(GTK_LABEL(chat_message), TRUE);
        gtk_label_set_xalign(GTK_LABEL(chat_message), 0.0);
        gtk_label_set_lines(GTK_LABEL(chat_message), 2);
        gtk_label_set_ellipsize(GTK_LABEL(chat_message), PANGO_ELLIPSIZE_END);
        gtk_widget_queue_draw(chat_message);
    } else {
        g_warning("chat_message not found in the chatblock.");
    }
}

static void update_chat_time(GtkWidget *chatblock, const char *new_time) {
    GtkWidget *chat_time = g_object_get_data(G_OBJECT(chatblock), "chat_time");

    if (chat_time != NULL) {
        gtk_label_set_text(GTK_LABEL(chat_time), new_time);

        gtk_widget_queue_draw(chat_time);
    } else {
        g_warning("chat_time not found in the chatblock.");
    }
}

static void update_avatar(GtkWidget *chatblock, t_chat_info *new_chat_info) {
    GtkWidget *current_avatar_wrapper = g_object_get_data(G_OBJECT(chatblock), "avatar_wrapper");

    if (current_avatar_wrapper != NULL) {
        gtk_container_remove(GTK_CONTAINER(chatblock), current_avatar_wrapper);
    }

    GtkWidget *new_avatar_wrapper = sidebar_create_avatar(new_chat_info);

    g_object_set_data(G_OBJECT(chatblock), "avatar_wrapper", new_avatar_wrapper);

    gtk_box_pack_start(GTK_BOX(chatblock), new_avatar_wrapper, FALSE, FALSE, 0);

    gtk_box_reorder_child(GTK_BOX(chatblock), new_avatar_wrapper, 0);
    gtk_widget_show_all(chatblock);
}


void update_chatblock(GtkWidget *event_box, t_chat_info *chat_info) {
    GtkWidget *chatblock = g_object_get_data(G_OBJECT(event_box), "chatblock");

    update_chat_message(chatblock, create_formatted_last_message(chat_info));
    update_chat_time(chatblock, create_chat_time_string(chat_info));
    update_avatar(chatblock, chat_info);

    GtkWidget *stretchable_box = g_object_get_data(G_OBJECT(vendor.pages.main_page.sidebar.widget), "stretchable_box");
    gtk_box_reorder_child(GTK_BOX(stretchable_box), event_box, 0);
}


GtkWidget *sidebar_create_chatblock(t_chat_info *chat_info) {
    GtkWidget *chatblock = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(chatblock, "sidebar__chatblock");
    gtk_widget_set_size_request(chatblock, 177, -1);
    GtkWidget *avatar_wrapper = vendor.pages.main_page.sidebar.create_avatar(chat_info);
    g_object_set_data(G_OBJECT(chatblock), "avatar_wrapper", avatar_wrapper);

    GtkWidget *chatblock_text = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(chatblock_text, "sidebar__chatblock_text");

    GtkWidget *chatblock_text_header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(chatblock_text_header, "sidebar__chatblock_chat__header");

    GtkWidget *chat_name = create_chat_name(chat_info);
    g_object_set_data(G_OBJECT(chatblock), "chat_name", chat_name);

    // Создаем метку с разметкой
    GtkWidget *chat_message = create_chat_message(chat_info);
    g_object_set_data(G_OBJECT(chatblock), "chat_message", chat_message);

    GtkWidget *chat_time = create_chat_time(chat_info);
    g_object_set_data(G_OBJECT(chatblock), "chat_time", chat_time);

    GtkWidget *spacer = gtk_label_new("");
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_widget_set_size_request(spacer, 0, -1);

    gtk_box_pack_start(GTK_BOX(chatblock), avatar_wrapper, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(chatblock_text), chatblock_text_header, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(chatblock_text), chat_message, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(chatblock_text_header), chat_name, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(chatblock_text_header), spacer, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(chatblock_text_header), chat_time, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(chatblock), chatblock_text, TRUE, TRUE, 0);

    GtkWidget *event_box = gtk_event_box_new();
    vendor.helpers.set_classname_and_id(event_box, "sidebar__chatblock_wrapper");
    g_object_set_data(G_OBJECT(event_box), "chat_info", chat_info);
    g_object_set_data(G_OBJECT(event_box), "chatblock", chatblock);
    g_signal_connect(event_box, "button-press-event", G_CALLBACK(click_handler), NULL);
    vendor.helpers.add_hover(event_box);
    gtk_container_add(GTK_CONTAINER(event_box), chatblock);

    return event_box;
}
