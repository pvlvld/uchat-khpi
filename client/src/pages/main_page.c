#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

GtkWidget *create_main_page(void) {
    gtk_window_set_title(GTK_WINDOW(vendor.window), "ShadowTalk | Cahts");
    GtkWidget *main_page = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *sidebar = vendor.pages.main_page.sidebar.init();

    GtkWidget *right_box = vendor.pages.main_page.chat.init();

    gtk_box_pack_start(GTK_BOX(main_page), sidebar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_page), right_box, TRUE, TRUE, 0);

    return main_page;
}
