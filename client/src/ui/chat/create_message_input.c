#include "../../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

static GtkWidget *scrolled_window = NULL;
static int line_count = 1;

static gboolean chat_on_input_focus_in(GtkWidget *text_view, GdkEventFocus *event, gpointer user_data) {
    (void)text_view;
    (void)event;
    GtkWidget *placeholder = (GtkWidget *)user_data;
    gtk_widget_hide(placeholder);
    return FALSE;
}

gboolean chat_on_input_focus_out(GtkWidget *text_view, GdkEventFocus *event, gpointer user_data) {
    (void)event;
    GtkWidget *placeholder = (GtkWidget *)user_data;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    if (g_strcmp0(text, "") == 0) {
        gtk_widget_show(placeholder);
    }
    g_free(text);

    return FALSE;
}

static void update_text_view_height(GtkTextView *text_view) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    PangoLayout *layout = gtk_widget_create_pango_layout(GTK_WIDGET(text_view), text);

    // Limit width so that strings are not infinitely long
    int scrolled_window_width = gtk_widget_get_allocated_width(scrolled_window) - 48;
    pango_layout_set_width(layout, scrolled_window_width * PANGO_SCALE);

    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);

    line_count = pango_layout_get_line_count(layout);

    int width, height;
    pango_layout_get_size(layout, &width, &height);

    int height_in_pixels = height / PANGO_SCALE + 10;

    gtk_widget_set_size_request(GTK_WIDGET(text_view), -1, height_in_pixels);

    if (height_in_pixels < 38) {
        height_in_pixels = 38;
    } else if (height_in_pixels > 200) {
         height_in_pixels = 200;
    } else {
        height_in_pixels += 6;
    }

    gtk_widget_set_size_request(scrolled_window, -1, height_in_pixels);

    g_free(text);
    g_object_unref(layout);
}

static int get_cursor_line(GtkTextView *text_view) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);

    GtkTextMark *insert_mark = gtk_text_buffer_get_insert(buffer);

    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, insert_mark);

    int line_number = gtk_text_iter_get_line(&iter);

    return line_number;
}

static gboolean scroll_to_height(gpointer data) {
    t_scroll_data *scroll_data = (t_scroll_data *)data;
    double new_value = gtk_adjustment_get_value(scroll_data->adjustment) + scroll_data->height;
    gtk_adjustment_set_value(scroll_data->adjustment, new_value);

    g_free(scroll_data);
    return FALSE;
}

static void send_message(GtkTextView *text_view) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    if (g_strcmp0(text, "") != 0) {
        // local encrypting
        char *encrypt = vendor.crypto.encrypt_data_for_db(vendor.crypto.public_key_str, text);
        if (encrypt) {
            t_messages_struct *message_struct = vendor.database.tables.messages_table.add_message(vendor.database.tables.messages_table.get_total_messages() + 1,
			vendor.active_chat.chat->id, vendor.current_user.user_id, encrypt, NULL, 0);
            ++vendor.pages.main_page.chat.temp_message_counter;
            add_chat_message(message_struct, 0);

            vendor.active_chat.chat->last_message = message_struct;

            update_chatblock(vendor.active_chat.chat_sidebar_widget, vendor.active_chat.chat, 1);
            free(encrypt);
        }

        encrypt = vendor.crypto.encrypt_data_for_db(vendor.active_chat.recipient_public_key, text);
        if (encrypt) {
            char buffer[50];
            sprintf(buffer, "%d", vendor.active_chat.chat->id);

            cJSON *json_body = cJSON_CreateObject();
            cJSON_AddStringToObject(json_body, "chat_id", buffer);
            cJSON_AddStringToObject(json_body, "message", encrypt);

            vendor.ssl_struct.send_request("POST", "/send_message", json_body);

            free(encrypt);
        }

        gtk_text_buffer_delete(buffer, &start, &end);
    } else {
        g_print("Empty message, not sending.\n");
    }

    g_free(text);
}


static gboolean chat_input_on_key_press(GtkWidget *text_view, GdkEventKey *event, gpointer user_data) {
    (void) user_data;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    int cursor_position = get_cursor_line(GTK_TEXT_VIEW(text_view));
    double line_height = gtk_widget_get_allocated_height(scrolled_window) / line_count;

    GtkAdjustment *v_adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
    t_scroll_data *scroll_data = g_malloc(sizeof(t_scroll_data));
    scroll_data->adjustment = v_adjustment;
    scroll_data->height = cursor_position * line_height;

    g_idle_add(scroll_to_height, scroll_data);

    if (event->keyval == GDK_KEY_Return && (event->state & GDK_SHIFT_MASK)) {
        gtk_text_buffer_insert_at_cursor(buffer, "\n", -1);
        update_text_view_height(GTK_TEXT_VIEW(text_view));
        return TRUE;
    }

    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
        send_message(GTK_TEXT_VIEW(text_view));
        return TRUE;
    }

    return FALSE;
}

static gboolean button_send_message_clicked(GtkWidget *widget, GtkWidget *text_view) {
    (void) widget;
    send_message(GTK_TEXT_VIEW(text_view));

    return FALSE;
}

// Tracking changes in the buffer for altitude updates
static void on_text_buffer_changed(GtkTextBuffer *buffer, gpointer user_data) {
    (void) buffer;
    GtkTextView *text_view = GTK_TEXT_VIEW(user_data);
    update_text_view_height(text_view);
}

GtkWidget *create_message_input(GtkWidget *message_send) {
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    vendor.helpers.set_classname_and_id(scrolled_window, "chat__value_scroll");
    gtk_widget_set_size_request(scrolled_window, -1, -1);

    GtkWidget *message_input_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(message_input_wrapper, -1, -1);
    vendor.helpers.set_classname_and_id(message_input_wrapper, "chat__value_wrapper");

    gtk_container_add(GTK_CONTAINER(scrolled_window), message_input_wrapper);

    GtkWidget *message_value = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(message_value, "chat__value");
    gtk_style_context_add_class(gtk_widget_get_style_context(message_value), "_chat__input_block");
    gtk_widget_set_size_request(message_value, -1, 38);
    gtk_widget_set_halign(message_value, GTK_ALIGN_FILL);

    GtkWidget *value_entry = gtk_text_view_new();
    vendor.helpers.set_classname_and_id(value_entry, "chat__value_input");

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(value_entry), GTK_WRAP_WORD_CHAR);  // Перенос текста
    gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(value_entry), FALSE); // Отключаем Tab для навигации
    gtk_box_pack_start(GTK_BOX(message_value), value_entry, TRUE, TRUE, 0);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(value_entry), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(value_entry), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(value_entry), 10);

    // Creating a placeholder
    GtkWidget *value_placeholder = gtk_label_new("Write a message");
    vendor.helpers.set_classname_and_id(value_placeholder, "chat__input_placeholder");
    gtk_style_context_add_class(gtk_widget_get_style_context(value_placeholder), "_chat__input_placeholder");
    gtk_widget_set_halign(value_placeholder, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(message_input_wrapper), message_value, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(message_input_wrapper), value_placeholder, TRUE, FALSE, 0);

    // Functions for showing/hiding placeholder
    g_signal_connect(value_entry, "focus-in-event", G_CALLBACK(chat_on_input_focus_in), value_placeholder);
    g_signal_connect(value_entry, "focus-out-event", G_CALLBACK(chat_on_input_focus_out), value_placeholder);

    // Set Shift+Enter handler and dynamic height change
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(value_entry));
    g_signal_connect(buffer, "changed", G_CALLBACK(on_text_buffer_changed), value_entry);
    g_signal_connect(value_entry, "key-press-event", G_CALLBACK(chat_input_on_key_press), NULL);

    g_signal_connect(message_send, "clicked", G_CALLBACK(button_send_message_clicked), value_entry);

    gtk_widget_show(value_placeholder);
    return scrolled_window;
}
