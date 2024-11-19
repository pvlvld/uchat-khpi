#include "../../../inc/header.h"

static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    (void) widget;
    (void) data;
    if (event->button == GDK_BUTTON_SECONDARY) {
        int x = event->x_root;
        int y = event->y_root;

        vendor.modal.message_info.show(GTK_WINDOW(gtk_widget_get_toplevel(widget)), x, y);
        // TODO: add logic for editing, deleting, copying and (?)replying

        return TRUE;
    }
    return FALSE;
}

GtkWidget *create_message_box(const char *message_txt, ssize_t username_length) {
    GtkWidget *message_text = gtk_text_view_new();
    g_signal_connect(message_text, "button-press-event", G_CALLBACK(on_button_press_event), NULL);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(message_text));
    gtk_text_buffer_set_text(buffer, message_txt, -1);

    if (username_length > 0) {
        GtkTextIter start, end;
        GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(buffer);
        GtkTextTag *color_tag = gtk_text_tag_new("username_color");
        g_object_set(color_tag, "foreground", "#047857", NULL);
        gtk_text_tag_table_add(tag_table, color_tag);

        gtk_text_buffer_get_iter_at_offset(buffer, &start, 0);
        gtk_text_buffer_get_iter_at_offset(buffer, &end, username_length);
        gtk_text_buffer_apply_tag(buffer, color_tag, &start, &end);
    }

    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(message_text), GTK_WRAP_CHAR);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(message_text), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(message_text), FALSE);

    return message_text;
}
