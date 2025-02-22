#include "../../inc/header.h"

static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    (void) widget;
    GtkWidget *image = GTK_WIDGET(user_data);
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));

    if (pixbuf) {
        int width = gdk_pixbuf_get_width(pixbuf);
        int height = gdk_pixbuf_get_height(pixbuf);

        cairo_save(cr);
        cairo_arc(cr, width / 2, height / 2, MIN(width, height) / 2, 0, 2 * G_PI);
        cairo_clip(cr);

        gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
        cairo_paint(cr);
        cairo_restore(cr);
    }

    return FALSE;
}

GtkWidget *create_avatar(const char *path, int width, int height) {
    char full_path[512];

    if (!g_str_has_suffix(path, ".jpg")) {
        snprintf(full_path, sizeof(full_path), "resources/images/avatars/%s.jpg", path);
    } else {
        snprintf(full_path, sizeof(full_path), "resources/images/avatars/%s", path);
    }

    if (!g_file_test(full_path, G_FILE_TEST_EXISTS)) {
        snprintf(full_path, sizeof(full_path), "resources/images/avatars/logo_%lu.jpg", strlen(path) % 4 + 1);
    }

    GtkWidget *image = gtk_image_new_from_file(full_path);
    gtk_widget_set_size_request(image, width, height);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, width, height);
    gtk_widget_set_valign(drawing_area, GTK_ALIGN_END);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), image);

    return drawing_area;
}

