#include "../../../inc/header.h"
#include <gtk/gtk.h>

GtkWidget *sidebar_create_avatar(int avatar_id, const gchar *image_path) {
    GtkWidget *avatar_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(avatar_wrapper, "avatar-wrapper");

    GtkWidget *avatar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(avatar, "avatar");

    gchar avatar_class_str[64];
    g_snprintf(avatar_class_str, sizeof(avatar_class_str), "avatar-class-%d", avatar_id);
    gtk_widget_set_name(avatar, "avatar");
    gtk_widget_set_size_request(avatar, 48, 48);

    gchar css[512];
    g_snprintf(css, sizeof(css), ".%s { background-image: url('file://%s');}", avatar_class_str, image_path);

    // Устанавливаем выравнивание
    gtk_widget_set_halign(avatar_wrapper, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(avatar_wrapper, GTK_ALIGN_CENTER);

    // Создаем CSS провайдер
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL); // Используйте gtk_css_provider_load_from_data для GTK 3

    // Применяем CSS к основному экрану
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(), // Изменено на gdk_screen_get_default()
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    // Вместо gtk_box_append используйте gtk_box_pack_start
    gtk_box_pack_start(GTK_BOX(avatar_wrapper), avatar, TRUE, TRUE, 0);

    return avatar_wrapper;
}
