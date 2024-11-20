#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

void popup_init(void) {
    GtkWidget *popup = gtk_window_new(GTK_WINDOW_POPUP);
    vendor.popup.window = popup;
    vendor.helpers.set_classname_and_id(popup, "_popup");
    gtk_window_set_decorated(GTK_WINDOW(popup), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(popup), FALSE);
    gtk_widget_set_visual(popup, gdk_screen_get_rgba_visual(gtk_widget_get_screen(popup)));

    GdkDisplay *display = gdk_display_get_default();
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    GdkRectangle monitor_geometry;
    gdk_monitor_get_geometry(monitor, &monitor_geometry);

    gint popup_x = monitor_geometry.width - 250;
    gint popup_y = 100;

    gtk_window_move(GTK_WINDOW(popup), popup_x, popup_y);

    GtkWidget *popup_block = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(popup_block, GTK_ALIGN_END);

    vendor.popup.block = popup_block;
    vendor.helpers.set_classname_and_id(popup_block, "_popup__block");
    gtk_container_add(GTK_CONTAINER(popup), popup_block);
}

void popup_update(void) {
    GdkDisplay *display = gdk_display_get_default();
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    GdkRectangle monitor_geometry;
    gdk_monitor_get_geometry(monitor, &monitor_geometry);

    gint popup_x = monitor_geometry.width - 250;
    gint popup_y = 100;
    gtk_window_move(GTK_WINDOW(vendor.popup.window), popup_x, popup_y);
    gtk_widget_set_size_request(vendor.popup.window, 240, monitor_geometry.height - 136);
}

void popup_hide(void) {
    gtk_widget_hide(vendor.popup.window);
}

void popup_show(void) {
    vendor.popup.update();
    gtk_widget_show_all(vendor.popup.window);
}

static gboolean remove_label(GtkWidget *label) {
    gtk_widget_destroy(label);

    GList *children = gtk_container_get_children(GTK_CONTAINER(vendor.popup.block));
    if (g_list_length(children) == 0) {
        popup_hide();
    }
    g_list_free(children);

    return FALSE;
}

static gboolean add_show(GtkWidget *label) {
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "_show");
    return FALSE;
}

static gboolean remove_show(GtkWidget *label) {
    gtk_style_context_remove_class(gtk_widget_get_style_context(label), "_show");
    return FALSE;
}

void popup_add_message(const char *message) {
    GtkWidget *label = gtk_label_new(message);
    vendor.helpers.set_classname_and_id(label, "_popup__message");

    gtk_box_pack_start(GTK_BOX(vendor.popup.block), label, FALSE, FALSE, 0);
    gtk_widget_show_all(vendor.popup.window);

    g_timeout_add(100, (GSourceFunc)add_show, label);
    g_timeout_add(3000, (GSourceFunc)remove_show, label);
    g_timeout_add(3500, (GSourceFunc)remove_label, label);
}

t_popup init_popup(void) {
    t_popup popup = {
        .window = NULL,
        .init = popup_init,
        .add_message = popup_add_message,
        .hide = popup_hide,
        .show = popup_show,
        .update = popup_update,
    };

    return popup;
}
