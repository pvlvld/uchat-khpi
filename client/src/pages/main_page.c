#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

GtkWidget *create_main_page(void) {
    gdk_window_set_cursor(gtk_widget_get_window(vendor.window), NULL);
    gtk_window_set_title(GTK_WINDOW(vendor.window), "ShadowTalk | Cahts");
    GtkWidget *main_page = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.pages.main_page.main_page = main_page;
    GtkWidget *sidebar = vendor.pages.main_page.sidebar.init();
    vendor.pages.main_page.sidebar.widget = sidebar;

    GtkWidget *right_box = vendor.pages.main_page.chat.no_chat_init();
    vendor.pages.main_page.chat.chat_box = right_box;

    vendor.pages.main_page.chat.page = 0;
    vendor.pages.main_page.chat.shown_messages = 0;
    vendor.pages.main_page.chat.temp_message_counter = 0;

    gtk_box_pack_start(GTK_BOX(main_page), sidebar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_page), right_box, TRUE, TRUE, 0);

    return main_page;
}
