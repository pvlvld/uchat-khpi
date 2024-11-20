#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

void change_page(e_page_type page) {
    if (vendor.pages.current_page == page) return;

    if (vendor.pages.current_page_widget != NULL) {
        gtk_container_remove(GTK_CONTAINER(vendor.window_content), vendor.pages.current_page_widget);
        vendor.pages.current_page_widget = NULL;
    }

    vendor.pages.current_page = page;

    switch (page) {
    case LOGIN_PAGE:
        vendor.pages.current_page_widget = create_login_page();
        break;
    case REGISTER_PAGE:
        vendor.pages.current_page_widget = create_register_page();
        break;
    case MAIN_PAGE:
        vendor.pages.current_page_widget = create_main_page();
        break;
    case LOADING_PAGE:
        vendor.pages.current_page_widget = create_loading_page();
        break;
    default:
        g_print("Attempted to change to an unknown page type: %d\n", page);
    }

    gtk_container_add(GTK_CONTAINER(vendor.window_content), vendor.pages.current_page_widget);
    gtk_widget_show_all(vendor.pages.current_page_widget);
}

t_pages init_pages(void) {
    t_pages pages = {
        .current_page = MAIN_PAGE,
        .current_page_widget = NULL,
        .change_page = change_page,
        .main_page = {
            .sidebar = init_sidebar(),
            .chat = init_chat(),
        },
        .login_page = init_login(),
        .register_page = init_register(),
    };
    return pages;
}
// g_timeout_add(200, (GSourceFunc)on_server_response_received, GINT_TO_POINTER(target_page));
