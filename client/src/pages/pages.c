#include "../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

void on_server_response_received(e_page_type target_page, gpointer user_data);

void perform_server_request(e_page_type target_page) {
    g_timeout_add(200, (GSourceFunc)on_server_response_received, GINT_TO_POINTER(target_page));
}

void on_server_response_received(e_page_type target_page, gpointer user_data) {
    (void) user_data;
    if (vendor.pages.current_page_widget != NULL) {
        gtk_container_remove(GTK_CONTAINER(vendor.window_content), vendor.pages.current_page_widget);
        vendor.pages.current_page_widget = NULL;
    }

    vendor.pages.current_page = target_page;
    switch (target_page) {
    case LOGIN_PAGE:
        vendor.pages.current_page_widget = create_login_page();
        break;
    case MAIN_PAGE:
        vendor.pages.current_page_widget = create_main_page();
        break;
    default:
        break;
    }

    gtk_container_add(GTK_CONTAINER(vendor.window_content), vendor.pages.current_page_widget);
    gtk_widget_show_all(vendor.pages.current_page_widget);
}

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
    case MAIN_PAGE:
        vendor.pages.current_page_widget = create_loading_page();
        perform_server_request(page);
    default:
        break;
    }

    gtk_container_add(GTK_CONTAINER(vendor.window_content), vendor.pages.current_page_widget);
    gtk_widget_show_all(vendor.pages.current_page_widget);
}

t_pages init_pages(void) {
    t_pages pages = {
        .current_page = LOGIN_PAGE,
        .current_page_widget = NULL,
        .change_page = change_page,
    };
    return pages;
}
