#include "../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = init_window(app);

    vendor.window_content = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    switch (vendor.pages.current_page) {
        case MAIN_PAGE:
            vendor.pages.current_page_widget = create_main_page();
            break;
        case LOGIN_PAGE:
            vendor.pages.current_page_widget = create_login_page();
            break;
        case REGISTER_PAGE:
            vendor.pages.current_page_widget = create_register_page();
            break;
        default:
            break;
    }

    gtk_container_add(GTK_CONTAINER(vendor.window_content), vendor.pages.current_page_widget);
    gtk_container_add(GTK_CONTAINER(window), vendor.window_content);

    gtk_widget_show_all(window);

    init_screen();

    if (user_data) return; // Plug
}

int main(int argc, char **argv) {
    if (init_server(argc, argv) != 1) return -1;
    srand(time(NULL));
    init_vendor(&vendor);
    vendor.database.create_database();
//    notify_init("ShadowTalk");
    GtkApplication *app;
    int status;
    app = gtk_application_new("org.example.GtkApplication", 0);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), 0, NULL);
    g_object_unref(app);

    return status;
}
