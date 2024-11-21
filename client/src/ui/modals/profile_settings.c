#include "../../../inc/header.h"
#include <gtk/gtk.h>

static void show_modal(GtkWindow *parent) {
    gint window_width, window_height;
    gint window_x, window_y;
    gtk_window_get_size(GTK_WINDOW(vendor.window), &window_width, &window_height);
    gtk_window_get_position(GTK_WINDOW(vendor.window), &window_x, &window_y);

    GtkWidget *dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Profile settings");
    vendor.helpers.set_classname_and_id(dialog, "modal__profile-settings");
    gtk_style_context_add_class(gtk_widget_get_style_context(dialog), "_modal-base");
    gtk_widget_set_visual(dialog, gdk_screen_get_rgba_visual(gtk_widget_get_screen(dialog)));

    gtk_window_set_default_size(GTK_WINDOW(dialog), 230, 143);

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);

    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), FALSE);

    gtk_window_move(GTK_WINDOW(dialog), window_x + 16, window_y + window_height - 143 + 24);

    GtkWidget *content = gtk_label_new("modal dialog.");
    gtk_container_add(GTK_CONTAINER(dialog), content);

    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    vendor.modal.profile_settings.window = dialog;
}

static void destroy_modal(void) {
    if (vendor.modal.profile_settings.window != NULL) {
        gtk_widget_destroy(vendor.modal.profile_settings.window);
        vendor.modal.profile_settings.window = NULL;
    }
}

t_modal_profile_settings init_modal_profile_settings(void) {
    t_modal_profile_settings profile_settings = {
        .window = NULL,
        .show = show_modal,
        .destroy = destroy_modal,
    };

    return profile_settings;
}
