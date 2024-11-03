#include "../../inc/header.h"

static gboolean enter_notify_event(GtkWidget *widget, GdkEventCrossing *event) {
    (void) event;
    if (vendor.active_chat.chat_sidebar_widget != widget) {
        gtk_style_context_add_class(gtk_widget_get_style_context(widget), "hover");
    }
    GdkCursor *cursor = gdk_cursor_new_for_display(gdk_display_get_default(), GDK_HAND2);
    gdk_window_set_cursor(gtk_widget_get_window(widget), cursor);
    g_object_unref(cursor);
    return TRUE;
}

static gboolean leave_notify_event(GtkWidget *widget, GdkEventCrossing *event) {
    (void) event;
    if (vendor.hover_chat.chat_sidebar_widget != widget)
        gtk_style_context_remove_class(gtk_widget_get_style_context(widget), "hover");
    gdk_window_set_cursor(gtk_widget_get_window(widget), NULL);
    return TRUE;
}

void add_hover(GtkWidget *event_box) {
    g_signal_connect(event_box, "enter-notify-event", G_CALLBACK(enter_notify_event), NULL);
    g_signal_connect(event_box, "leave-notify-event", G_CALLBACK(leave_notify_event), NULL);
}
