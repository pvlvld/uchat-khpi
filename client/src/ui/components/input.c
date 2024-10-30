#include "../../../inc/header.h"
#include <gtk/gtk.h>

// Default handler for input, if the user has not passed their
static void default_input_handler(GtkEntry *entry, gpointer user_data) {
    // Получение текста из GtkEntry
    const gchar *text = gtk_entry_get_text(entry);
    g_print("The text (default): %s\n", text);
    // Очистка текста в GtkEntry
    gtk_entry_set_text(entry, "");
    if (user_data) return;  // Plug for user_data
}

GtkWidget *create_input(const gchar *name, const gchar *placeholder, GCallback input_handler) {
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    gtk_widget_set_name(entry, name);
    gtk_style_context_add_class(gtk_widget_get_style_context(entry), name);

    if (input_handler == NULL) {
        g_signal_connect(entry, "activate", G_CALLBACK(default_input_handler), NULL);
    } else {
        g_signal_connect(entry, "activate", input_handler, NULL);
    }

    return entry;
}
