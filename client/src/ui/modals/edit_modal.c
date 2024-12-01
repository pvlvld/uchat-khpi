#include "../../../inc/header.h"
#include <gtk/gtk.h>

static gboolean on_timeout(gpointer user_data) {
    (void) user_data;

    vendor.modal.message_info.edit_modal.is_open = 1;

    return FALSE;
}

static GtkWidget *scrolled_window = NULL;
static int line_count = 1;

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

static void update_message(GtkTextView *text_view, t_message_info_modal *message_info) {
    (void) message_info;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    if (g_strcmp0(text, "") != 0) {

        char *encrypt = vendor.crypto.encrypt_data_for_db(vendor.crypto.public_key_str, text);
        if (encrypt) {
            t_messages_struct *message_struct = vendor.database.tables.messages_table.edit_message_and_get(message_info->info->message_id, message_info->info->chat_id, encrypt);
            redraw_message_wrapper(message_info->info, message_struct);

            if (vendor.active_chat.chat->last_message->message_id == message_struct->message_id
                && vendor.active_chat.chat->id == (unsigned int) message_struct->chat_struct->chat_id) {
                	vendor.active_chat.chat->last_message = message_struct;
            		update_chatblock(vendor.active_chat.chat_sidebar_widget, vendor.active_chat.chat);
                }
            free(encrypt);
        }

        gtk_text_buffer_delete(buffer, &start, &end);
        vendor.modal.message_info.edit_modal.destroy();
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

    if (event->keyval == GDK_KEY_Return && (event->state & GDK_SHIFT_MASK)) {
        gtk_text_buffer_insert_at_cursor(buffer, "\n", -1);
        update_text_view_height(GTK_TEXT_VIEW(text_view));
        return TRUE;
    }

    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
        t_message_info_modal *message_info = (t_message_info_modal *)user_data;
        update_message(GTK_TEXT_VIEW(text_view), message_info);
        return TRUE;
    }

    return FALSE;
}

// Tracking changes in the buffer for altitude updates
static void on_text_buffer_changed(GtkTextBuffer *buffer, gpointer user_data) {
    (void) buffer;
    GtkTextView *text_view = GTK_TEXT_VIEW(user_data);
    update_text_view_height(text_view);
}

static GtkWidget *create_edit_input(t_message_info_modal *message_info) {
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, -1, -1);

    GtkWidget *message_input_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(message_input_wrapper, -1, -1);

    gtk_container_add(GTK_CONTAINER(scrolled_window), message_input_wrapper);

    GtkWidget *message_value = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(message_value, -1, 38);
    gtk_widget_set_halign(message_value, GTK_ALIGN_FILL);

    GtkWidget *value_entry = gtk_text_view_new();
    message_info->value_entry = value_entry;

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(value_entry), GTK_WRAP_WORD_CHAR);  // Перенос текста
    gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(value_entry), FALSE); // Отключаем Tab для навигации
    gtk_box_pack_start(GTK_BOX(message_value), value_entry, TRUE, TRUE, 0);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(value_entry), 10);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(value_entry), 10);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(value_entry), 10);

    gtk_box_pack_start(GTK_BOX(message_input_wrapper), message_value, TRUE, TRUE, 0);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(value_entry));
    g_signal_connect(buffer, "changed", G_CALLBACK(on_text_buffer_changed), value_entry);
    g_signal_connect(value_entry, "key-press-event", G_CALLBACK(chat_input_on_key_press), message_info);

    gtk_text_buffer_set_text(buffer, message_info->text, -1);

    return scrolled_window;
}

static void on_edit_button_clicked(GtkButton *button, gpointer user_data) {
    (void) button;
    t_message_info_modal *message_info = (t_message_info_modal *)user_data;

    GtkWidget *text_view = message_info->value_entry;

    update_message(GTK_TEXT_VIEW(text_view), message_info);
}


static void show_modal(GtkWindow *parent, t_message_info_modal *message_info) {
    (void) message_info;
    const int WIDTH = 500;
    const int HEIGHT = 500;
    GtkWidget *dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Edit message");
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

    GtkWidget *title = gtk_label_new("Edit message:");
    gtk_box_pack_start(GTK_BOX(wrapper), title, FALSE, FALSE, 0);
    vendor.helpers.set_classname_and_id(title, "modal__edit_title");

    GtkWidget *message_input = create_edit_input(message_info);
    gtk_box_pack_start(GTK_BOX(wrapper), message_input, TRUE, TRUE, 20);
    vendor.helpers.set_classname_and_id(message_input, "modal__edit_input");

    GtkWidget *button = gtk_button_new_with_label("Edit");
//    vendor.helpers.set_classname_and_id(button, "auth__button");
    g_signal_connect(button, "clicked", G_CALLBACK(on_edit_button_clicked), message_info);
    vendor.helpers.add_hover(button);
    vendor.helpers.set_classname_and_id(button, "modal__edit_button");
    gtk_box_pack_start(GTK_BOX(wrapper), button, FALSE, FALSE, 0);

    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    vendor.modal.message_info.edit_modal.window = dialog;
    vendor.modal.message_info.destroy();
    g_timeout_add(100, on_timeout, NULL);
}

static void destroy_modal(void) {
    if (vendor.modal.message_info.edit_modal.window != NULL && vendor.modal.message_info.edit_modal.is_open) {
        gtk_widget_destroy(vendor.modal.message_info.edit_modal.window);
        vendor.modal.message_info.edit_modal.window = NULL;
        vendor.modal.message_info.edit_modal.is_open = 0;
    }
}

t_modal_edit_modal init_edit_modal(void) {
    t_modal_edit_modal edit_modal = {
        .is_open = 0,
        .window = NULL,
        .show = show_modal,
        .destroy = destroy_modal,
    };

    return edit_modal;
}
