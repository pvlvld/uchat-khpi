#include "../../../inc/header.h"

static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    (void)widget;
    (void)data;
    if (event->button == GDK_BUTTON_SECONDARY) {
        int x = event->x_root; // x for modal
        int y = event->y_root; // y for modal

        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
        gchar *text = NULL;
        gboolean is_full = 1;

        GtkTextIter start, end;
        if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
            is_full = 0;
            text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        } else {
            GtkTextIter start_iter, end_iter;
            gtk_text_buffer_get_start_iter(buffer, &start_iter);
            gtk_text_buffer_get_end_iter(buffer, &end_iter);
            text = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);
        }

        t_message_info_struct *message_info_struct = (t_message_info_struct *)data;

        vendor.modal.message_info.show(GTK_WINDOW(gtk_widget_get_toplevel(widget)), message_info_struct, x, y, text, is_full);
        return TRUE;
    }
    return FALSE;
}

GtkWidget *create_message_box(const char *message_txt, t_message_info_struct *message_info_struct) {
    GtkWidget *message_text = gtk_text_view_new();
    g_signal_connect(message_text, "button-press-event", G_CALLBACK(on_button_press_event), message_info_struct);
    vendor.helpers.set_classname_and_id(message_text, "chat__message__text");

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_text));
    gtk_text_buffer_set_text(buffer, message_txt, -1);

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(message_text), GTK_WRAP_CHAR);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(message_text), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(message_text), FALSE);

    return message_text;
}
