#include "../../../inc/header.h"
#include <gtk/gtk.h>

static char *format_last_message(const char *sender_name, const char *message) {
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

GtkWidget *sidebar_create_chatblock(t_chat_info *chat_info) {
    GtkWidget *chatblock = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(chatblock, "sidebar__chatblock");
    gtk_widget_set_size_request(chatblock, 177, -1);
    GtkWidget *avatar_wrapper = vendor.pages.main_page.sidebar.create_avatar(chat_info);

    GtkWidget *chatblock_text = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(chatblock_text, "sidebar__chatblock_text");

    GtkWidget *chatblock_text_header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(chatblock_text_header, "sidebar__chatblock_chat__header");

    // Получаем имя группы для группового чата
    if (chat_info->type == 1) { // Групповой чат
        chat_info->name = get_group_name_by_chat_id(chat_info->id);
    }

    GtkWidget *chat_name = gtk_label_new(chat_info->name);
    vendor.helpers.set_classname_and_id(chat_name, "sidebar__chatblock_chat__name");
    gtk_label_set_lines(GTK_LABEL(chat_name), 1);
    gtk_label_set_ellipsize(GTK_LABEL(chat_name), PANGO_ELLIPSIZE_END);

    // Проверка значений на NULL и создание копий строк
    char *last_message_copy = g_strdup(chat_info->last_message ? chat_info->last_message : "");
    char *sender_name_copy = g_strdup(chat_info->sender_name ? chat_info->sender_name : "");

    // Убираем лишние пробелы
    char *stripped_last_message = g_strstrip(last_message_copy);
    char *stripped_sender_name = g_strstrip(sender_name_copy);

    // Форматируем сообщение
    char *formatted_last_message = NULL;
    if (chat_info->type == 0) {  // Приватный чат
        formatted_last_message = g_strdup(stripped_last_message);
    } else {  // Групповой чат
        formatted_last_message = format_last_message(stripped_sender_name, stripped_last_message, chat_info->type);
    }

    // Создаем метку с разметкой
    GtkWidget *chat_message = gtk_label_new(formatted_last_message ? formatted_last_message : "");
    gtk_label_set_use_markup(GTK_LABEL(chat_message), TRUE);
    gtk_label_set_line_wrap(GTK_LABEL(chat_message), TRUE);
    gtk_label_set_lines(GTK_LABEL(chat_message), 2);
    gtk_label_set_ellipsize(GTK_LABEL(chat_message), PANGO_ELLIPSIZE_END);
    vendor.helpers.set_classname_and_id(chat_message, "sidebar__chatblock_chat__message");

    GtkWidget *chat_time = gtk_label_new(format_timestamp(chat_info->timestamp));
    vendor.helpers.set_classname_and_id(chat_time, "sidebar__chatblock_chat__time");

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
    g_signal_connect(event_box, "button-press-event", G_CALLBACK(click_handler), NULL);
    vendor.helpers.add_hover(event_box);
    gtk_container_add(GTK_CONTAINER(event_box), chatblock);

    // Освобождаем память
    g_free(last_message_copy);
    g_free(sender_name_copy);
    g_free(formatted_last_message);

    return event_box;
}
