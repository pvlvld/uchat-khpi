#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

GdkScreen *init_screen(void) {
    GtkCssProvider *css_provider = init_css_provider("resources/styles/style.css");

    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(css_provider);
    return screen;
}
