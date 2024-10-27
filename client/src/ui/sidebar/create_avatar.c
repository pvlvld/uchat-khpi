#include "../../../inc/header.h"
#include <gtk/gtk.h>

GtkWidget *sidebar_create_avatar(int avatar_id, const gchar *image_path) {
    (void)avatar_id;
    (void)image_path;

    GtkWidget *fixed_container = gtk_fixed_new();
    gtk_widget_set_size_request(fixed_container, 55, 48);
    gtk_widget_set_name(fixed_container, "fixed-container");

    GtkWidget *avatar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(avatar, "avatar");

    gchar avatar_class_str[64];
    g_snprintf(avatar_class_str, sizeof(avatar_class_str), "avatar-class-%d", avatar_id);
    gtk_widget_set_name(avatar, "avatar");
    gtk_style_context_add_class(gtk_widget_get_style_context(avatar), avatar_class_str);
    gtk_widget_set_size_request(avatar, 48, 48);

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    gchar css[512];
    g_snprintf(css, sizeof(css), ".%s { background-image: url('file://%s/resources/images/static/%s'); border-radius: 50%%; }", avatar_class_str, cwd, image_path);

    gtk_fixed_put(GTK_FIXED(fixed_container), avatar, 0, 0);

    GtkWidget *group_type = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(avatar, "group-type");
    gtk_style_context_add_class(gtk_widget_get_style_context(group_type), "single");
    gtk_widget_set_size_request(group_type, 14, 14);
    gtk_fixed_put(GTK_FIXED(fixed_container), group_type, 0, 0);

    GtkWidget *number_of_messages = gtk_label_new("100");
    gtk_widget_set_name(number_of_messages, "number-of-messages");
    gtk_widget_set_size_request(number_of_messages, 30, 16);
    gtk_fixed_put(GTK_FIXED(fixed_container), number_of_messages, 25, 32);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    return fixed_container;
}
