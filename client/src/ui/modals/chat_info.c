#include "../../../inc/header.h"
#include <gtk/gtk.h>

void show_modal(GtkWindow *parent, int x, int y) {
    GtkWidget *dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Chat settings");
    vendor.helpers.set_classname_and_id(dialog, "modal__chat-info");

    gtk_window_set_default_size(GTK_WINDOW(dialog), 143, 230);

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);

    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), FALSE);

    gtk_window_move(GTK_WINDOW(dialog), x, y - 40 < 900 ? y - 40 : 840);

    GtkWidget *content = gtk_label_new("modal dialog.");
    gtk_container_add(GTK_CONTAINER(dialog), content);

    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    vendor.modal.chat_info.window = dialog;
}

void destroy_modal(void) {
    if (vendor.modal.chat_info.window != NULL) {
        gtk_widget_destroy(vendor.modal.chat_info.window);
        vendor.modal.chat_info.window = NULL;
    }
}

t_modal_chat_ifno init_modal_chat_info(void) {
    t_modal_chat_ifno chat_ifno = {
        .window = NULL,
        .show = show_modal,
        .destroy = destroy_modal,
    };

    return chat_ifno;
}
