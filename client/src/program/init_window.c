#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

static gboolean on_focus_in_event(GtkWidget *widget, GdkEventFocus *event) {
    (void)widget;
    (void)event;
    //    g_print("Main window has gained focus\n");
    vendor.modal.chat_info.destroy();
    if (vendor.hover_chat.chat_sidebar_widget != NULL) {
        gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.hover_chat.chat_sidebar_widget), "hover");
        vendor.hover_chat.chat_sidebar_widget = NULL;
    }
    return FALSE;
}

static gboolean on_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    (void)widget;
    (void)event;
    (void)user_data;
    if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_m) {
        vendor.pages.change_page(MAIN_PAGE);
        return TRUE;
    }
    if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_KEY_l) {
        vendor.pages.change_page(LOGIN_PAGE);
        return TRUE;
    }
    return FALSE;
}

GtkWidget *init_window(GtkApplication *app) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "ShadowTalk");
    gtk_window_set_default_size(GTK_WINDOW(window), 1080, 720);
    gtk_widget_set_size_request(window, 900, 600);

    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press_event), vendor.window_content);
    g_signal_connect(window, "focus-in-event", G_CALLBACK(on_focus_in_event), NULL);

    return window;
}
