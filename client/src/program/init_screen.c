#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

//void on_style_updated(GtkWidget *widget, gpointer user_data) {
//    (void) user_data;
//    (void) widget;
//}

GdkScreen *init_screen(void) {
    GtkCssProvider *css_provider = init_css_provider("resources/styles/style.css");

    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
//    g_signal_connect(vendor.window, "style-updated", G_CALLBACK(on_style_updated), NULL);

    g_object_unref(css_provider);
    return screen;
}
