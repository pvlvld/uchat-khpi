#include "../../../inc/header.h"
#include <gtk/gtk.h>

static void custom_input_handler(GtkEntry *entry, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(entry);
    g_print("Custom Handler: Typed Text: %s\n", text);

    if (user_data) {
        g_print("Supplemental Data: %s\n", (const gchar *)user_data);
    }

    gtk_entry_set_text(entry, "");
}

static GtkWidget *init_search(void) {
    GtkWidget *entry_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    set_classname_and_id(entry_wrapper, "sidebar__search_wrapper");
    GtkWidget *entry = vendor.components.input.create("sidebar__search", "Search", G_CALLBACK(custom_input_handler));

    gtk_box_pack_start(GTK_BOX(entry_wrapper), entry, TRUE, TRUE, 0);
    return entry_wrapper;
}

static gboolean key_press_handler(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    (void) widget;
    (void) user_data;

    if (event->keyval == GDK_KEY_Escape) {
        g_print("Esc pressed\n");
        if (vendor.active_chat.chat_sidebar_widget != NULL) {
            gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.active_chat.chat_sidebar_widget), "active");
            vendor.active_chat.chat_sidebar_widget = NULL;
            vendor.active_chat.chat = NULL;
        }
        return TRUE;
    }

    if (event->keyval == GDK_KEY_space) {
        g_print("name: %s\n", vendor.active_chat.chat->name);
        return TRUE;
    }

    return FALSE;
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
        gtk_style_context_add_class(gtk_widget_get_style_context(vendor.active_chat.chat_sidebar_widget), "active");
    }

    return TRUE;
}

static gboolean enter_notify_event(GtkWidget *widget, GdkEventCrossing *event) {
    (void) event;
    gtk_style_context_add_class(gtk_widget_get_style_context(widget), "hover");
    GdkCursor *cursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_HAND2);
    gdk_window_set_cursor(gtk_widget_get_window(widget), cursor);
    g_object_unref(cursor);
    return TRUE;
}

static gboolean leave_notify_event(GtkWidget *widget, GdkEventCrossing *event) {
    (void) event;
    gtk_style_context_remove_class(gtk_widget_get_style_context(widget), "hover");
    gdk_window_set_cursor(gtk_widget_get_window(widget), NULL);
    return TRUE;
}

char *format_last_message(const char *sender_name, const char *message) {
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

GtkWidget *create_chatblock(t_chat_info *chat_info) {
    GtkWidget *chatblock = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    set_classname_and_id(chatblock, "sidebar__chatblock");
    gtk_widget_set_size_request(chatblock, 177, -1);
    GtkWidget *avatar_wrapper = vendor.sidebar.create_avatar(chat_info);

    GtkWidget *chatblock_text = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    set_classname_and_id(chatblock_text, "sidebar__chatblock_text");

    GtkWidget *chatblock_text_header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    set_classname_and_id(chatblock_text_header, "sidebar__chatblock_chat__header");

    GtkWidget *chat_name = gtk_label_new(chat_info->name);
    set_classname_and_id(chat_name, "sidebar__chatblock_chat__name");
    gtk_label_set_lines(GTK_LABEL(chat_name), 1);
    gtk_label_set_ellipsize(GTK_LABEL(chat_name), PANGO_ELLIPSIZE_END);

    // Форматируем сообщение
    char *formatted_last_message = format_last_message(chat_info->sender_name, chat_info->last_message);

    // Создаем метку с разметкой
    GtkWidget *chat_message = gtk_label_new(chat_info->type == 0 ? chat_info->last_message : formatted_last_message);
    gtk_label_set_use_markup(GTK_LABEL(chat_message), TRUE);
    gtk_label_set_line_wrap(GTK_LABEL(chat_message), TRUE);
    gtk_label_set_lines(GTK_LABEL(chat_message), 2);
    gtk_label_set_ellipsize(GTK_LABEL(chat_message), PANGO_ELLIPSIZE_END);
    set_classname_and_id(chat_message, "sidebar__chatblock_chat__message");

    GtkWidget *chat_time = gtk_label_new(format_timestamp(chat_info->timestamp));
    set_classname_and_id(chat_time, "sidebar__chatblock_chat__time");

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
    set_classname_and_id(event_box, "sidebar__chatblock_wrapper");
    g_object_set_data(G_OBJECT(event_box), "chat_info", chat_info);
    g_signal_connect(event_box, "button-press-event", G_CALLBACK(click_handler), NULL);
    g_signal_connect(event_box, "enter-notify-event", G_CALLBACK(enter_notify_event), NULL);
    g_signal_connect(event_box, "leave-notify-event", G_CALLBACK(leave_notify_event), NULL);
    gtk_container_add(GTK_CONTAINER(event_box), chatblock);

    // Освобождение памяти
    g_free(formatted_last_message);

    return event_box;
}

GtkWidget *sidebar_init(void) {
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(sidebar, "sidebar");
    gtk_widget_set_size_request(sidebar, 260, -1);
    gtk_widget_set_hexpand(sidebar, FALSE);

    GtkWidget *search = init_search();
    gtk_box_pack_start(GTK_BOX(sidebar), search, FALSE, FALSE, 0);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkWidget *stretchable_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(stretchable_box, TRUE);

    gtk_container_add(GTK_CONTAINER(scrolled_window), stretchable_box);

    gtk_box_pack_start(GTK_BOX(sidebar), scrolled_window, TRUE, TRUE, 0);

    t_chat_info **chats_info = parse_chats_info();
    qsort(chats_info, 12, sizeof(t_chat_info *), compare_chats);

    for (int i = 0; chats_info[i] != NULL; i++) {
        GtkWidget *avatar = create_chatblock(chats_info[i]);
        gtk_box_pack_start(GTK_BOX(stretchable_box), avatar, FALSE, FALSE, 0);
    }

    gtk_widget_set_vexpand(scrolled_window, TRUE);

    GtkWidget *fixed_height_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(fixed_height_box, -1, 80);
    gtk_widget_set_name(fixed_height_box, "fixed-height-box");

    GtkWidget *fixed_label = gtk_label_new("Fixed Height Box");
    gtk_box_pack_start(GTK_BOX(fixed_height_box), fixed_label, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(sidebar), fixed_height_box, FALSE, FALSE, 0);

    g_signal_connect(sidebar, "key-press-event", G_CALLBACK(key_press_handler), NULL);

    return sidebar;
}

t_sidebar init_sidebar(void) {
    t_sidebar sidebar = {
        .init = sidebar_init,
        .create_avatar = chatblock_create_avatar
    };
    return sidebar;
}
