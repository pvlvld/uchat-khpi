#include "../../../inc/header.h"
#include <gtk/gtk.h>

static void show_modal(GtkWindow *parent, int x, int y) {
    int width = 200;
    int height = 200;
    int window_width;
    gtk_window_get_size(GTK_WINDOW(vendor.window), &window_width, NULL);
    GtkWidget *dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Chat settings");
    vendor.helpers.set_classname_and_id(dialog, "modal__message-info");

    gtk_window_set_default_size(GTK_WINDOW(dialog), width, height);

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);

    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), FALSE);

    gtk_window_move(GTK_WINDOW(dialog), x + width < window_width + 54 ? x : window_width - width + 54, y - 40 < 900 ? y - 40 : 840);

    GtkWidget *content = gtk_label_new("modal dialog.");
    gtk_container_add(GTK_CONTAINER(dialog), content);

    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    vendor.modal.message_info.window = dialog;
}

static void destroy_modal(void) {
    if (vendor.modal.message_info.window != NULL) {
        gtk_widget_destroy(vendor.modal.message_info.window);
        vendor.modal.message_info.window = NULL;
    }
}

t_modal_message_info init_modal_message_info(void) {
    t_modal_message_info message_info = {
        .window = NULL,
        .show = show_modal,
        .destroy = destroy_modal,
    };

    return message_info;
}
