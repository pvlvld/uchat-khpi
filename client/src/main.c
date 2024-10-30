#include "../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>  // Добавлено для определения GdkEventButton

//static gboolean on_button_press_event(GtkWidget *widget, GdkEventButton *event) {
//	(void)widget;
//    if (event->type == GDK_BUTTON_PRESS) {
//        g_print("Window clicked at (%.2f, %.2f)\n", event->x, event->y);
//    }
//    return TRUE;
//}

static gboolean on_focus_in_event(GtkWidget *widget, GdkEventFocus *event) {
    (void)widget;
    (void)event;
//    g_print("Main window has gained focus\n");
    vendor.modal.chat_info.destroy();
    if (vendor.hover_chat.chat_sidebar_widget != NULL) {
        gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.hover_chat.chat_sidebar_widget), "hover");
        vendor.hover_chat.chat_sidebar_widget = NULL;
    }
    return FALSE; // Вернуть FALSE, чтобы другие обработчики могли обработать событие
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *sidebar;
    GtkWidget *right_box;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "ShadowTalk");
    gtk_window_set_default_size(GTK_WINDOW(window), 1080, 720);
    gtk_widget_set_size_request(window, 900, 600);

//    g_signal_connect(window, "button-press-event", G_CALLBACK(on_button_press_event), NULL);
    g_signal_connect(window, "focus-in-event", G_CALLBACK(on_focus_in_event), NULL);

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    sidebar = vendor.sidebar.init();
    right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *label = gtk_label_new("Select a chat to start messaging");

    gtk_widget_set_halign(label, GTK_ALIGN_FILL);
    gtk_widget_set_valign(label, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_widget_set_vexpand(label, TRUE);

    gtk_box_pack_start(GTK_BOX(right_box), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), sidebar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), right_box, TRUE, TRUE, 0);

    GtkCssProvider *css_provider = gtk_css_provider_new();
    GError *error = NULL;
    gtk_css_provider_load_from_path(css_provider, "resources/styles/style.css", &error); // Загрузка из файла

    if (error) {
        g_printerr("Error loading CSS file: %s\n", error->message);
        g_error_free(error);
    }

    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_set_name(right_box, "right-box");

    gtk_container_add(GTK_CONTAINER(window), box);

    gtk_widget_show_all(window);

    g_object_unref(css_provider);
    if (user_data) return; // Plug
}

int main(int argc, char **argv) {
    srand(time(NULL));
    init_vendor(&vendor);
    notify_init("ShadowTalk");
    GtkApplication *app;
    int status;
    app = gtk_application_new("org.example.GtkApplication", 0);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
