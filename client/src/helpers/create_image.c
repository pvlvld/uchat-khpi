#include "../../inc/header.h"

static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    (void) widget;
    GtkWidget *image = GTK_WIDGET(user_data);
    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));

    if (pixbuf) {
        int width = gdk_pixbuf_get_width(pixbuf);
        int height = gdk_pixbuf_get_height(pixbuf);

        int corner_radius = 8;

        cairo_save(cr);

        cairo_move_to(cr, corner_radius, 0);
        cairo_line_to(cr, width - corner_radius, 0);
        cairo_curve_to(cr, width, 0, width, corner_radius, width, corner_radius);
        cairo_line_to(cr, width, height - corner_radius);
        cairo_curve_to(cr, width, height, width - corner_radius, height, width - corner_radius, height);
        cairo_line_to(cr, corner_radius, height);
        cairo_curve_to(cr, 0, height, 0, height - corner_radius, 0, height - corner_radius);
        cairo_line_to(cr, 0, corner_radius);
        cairo_curve_to(cr, 0, 0, corner_radius, 0, corner_radius, 0);

        cairo_clip(cr);

        gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
        cairo_paint(cr);
        cairo_restore(cr);
    }

    return FALSE;
}


GtkWidget *create_image(const char *path, GtkWidget *message, int width) {
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "resources/images/images/%s", path);
    GtkWidget *image = gtk_image_new_from_file(full_path);

    GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
    int original_width = gdk_pixbuf_get_width(pixbuf);
    int original_height = gdk_pixbuf_get_height(pixbuf);

    int target_width = width;
    int target_height = (original_height * target_width) / original_width;

    if (original_width < target_width * 2) {
        target_width = original_width * 2;
        target_height = (original_height * target_width) / original_width;
    }
    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, target_width, target_height, GDK_INTERP_BILINEAR);

    gtk_image_set_from_pixbuf(GTK_IMAGE(image), scaled_pixbuf);
    gtk_widget_set_size_request(image, target_width, target_height);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(drawing_area, target_width, target_height);
    gtk_widget_set_valign(drawing_area, GTK_ALIGN_END);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), image);

    gtk_widget_set_size_request(message, target_width + 20, target_height + 40);
    return drawing_area;
}
