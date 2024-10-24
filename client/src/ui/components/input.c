#include "../../../inc/header.h"
#include <gtk/gtk.h>

// Default handler for input, if the user has not passed their
static void default_input_handler(GtkEntry *entry, gpointer user_data) {
    const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    g_print("The text (default): %s\n", text);
    gtk_editable_set_text(GTK_EDITABLE(entry), "");
    if (user_data) return;  // Plug for user_data
}

GtkWidget *create_input(const gchar *id, const gchar *placeholder, GCallback input_handler) {
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    gtk_widget_set_name(entry, id);

    if (input_handler == NULL) {
        g_signal_connect(entry, "activate", G_CALLBACK(default_input_handler), NULL);
    } else {
        g_signal_connect(entry, "activate", input_handler, NULL);
    }

    return entry;
}
