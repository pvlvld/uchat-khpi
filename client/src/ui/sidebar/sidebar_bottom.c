#include "../../../inc/header.h"
#include <gtk/gtk.h>

static void on_user_login_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    (void) widget;
    (void) user_data;
    if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
		GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
		gtk_clipboard_set_text(clipboard, vendor.current_user.user_login, -1);

        vendor.popup.add_message("Added to clipboard");
    }
}

static void on_settings_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    (void) widget;
    (void) event;
    (void) user_data;
    // vendor.modal.profile_settings.show(GTK_WINDOW(gtk_widget_get_toplevel(widget)));
}

static void on_create_group_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    (void) widget;
    (void) event;
    (void) user_data;
    vendor.modal.create_group.show(GTK_WINDOW(gtk_widget_get_toplevel(widget)));
}

static GtkWidget *create_profile_text_block(void) {
    GtkWidget *profile_text_block = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(profile_text_block, 152, 42);
    gtk_widget_set_margin_start(profile_text_block, 56);
    gtk_widget_set_margin_end(profile_text_block, 208);
    vendor.helpers.set_classname_and_id(profile_text_block, "sidebar__bottom_profile-block__text-block");

    GtkWidget *uesrname = gtk_label_new(vendor.current_user.username);
    vendor.helpers.set_classname_and_id(uesrname, "sidebar__bottom_profile-block__text-block_username");
    gtk_label_set_line_wrap(GTK_LABEL(uesrname), TRUE);
    gtk_label_set_lines(GTK_LABEL(uesrname), 1);
    gtk_widget_set_halign(uesrname, GTK_ALIGN_START);
	gtk_widget_set_valign(uesrname, GTK_ALIGN_START);
	gtk_label_set_xalign(GTK_LABEL(uesrname), 0.0);

    gtk_label_set_ellipsize(GTK_LABEL(uesrname), PANGO_ELLIPSIZE_END);
    gtk_box_pack_start(GTK_BOX(profile_text_block), uesrname, FALSE, FALSE, 0);

    GtkWidget *user_login_event_box = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(profile_text_block), user_login_event_box, FALSE, FALSE, 0);
    char user_login_text[50];
    snprintf(user_login_text, sizeof(user_login_text), "@%s", vendor.current_user.user_login);
    GtkWidget *user_login = gtk_label_new(user_login_text);
    vendor.helpers.set_classname_and_id(user_login_event_box, "sidebar__bottom_profile-block__text-block_user-login");
    vendor.helpers.add_hover(user_login);
    gtk_label_set_line_wrap(GTK_LABEL(user_login), TRUE);
    gtk_label_set_lines(GTK_LABEL(user_login), 1);
    gtk_widget_set_halign(user_login, GTK_ALIGN_START);
	gtk_widget_set_valign(user_login, GTK_ALIGN_START);
	gtk_label_set_xalign(GTK_LABEL(user_login), 0.0);
    gtk_label_set_ellipsize(GTK_LABEL(user_login), PANGO_ELLIPSIZE_END);
    gtk_container_add(GTK_CONTAINER(user_login_event_box), user_login);
    g_signal_connect(user_login_event_box, "button-press-event", G_CALLBACK(on_user_login_clicked), NULL);
    vendor.helpers.add_hover(user_login_event_box);

    return profile_text_block;
}

GtkWidget *sidebar_create_bottom(void) {
    GtkWidget *box = gtk_fixed_new();
    gtk_widget_set_size_request(box, 260, 110);
    vendor.helpers.set_classname_and_id(box, "sidebar__bottom");
    gtk_widget_set_hexpand(box, FALSE);

    GtkWidget *button_block = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(button_block, 230, 32);
    vendor.helpers.set_classname_and_id(button_block, "sidebar__bottom_button-block");

    // Используйте gtk_fixed_put для добавления button_block в box
    gtk_fixed_put(GTK_FIXED(box), button_block, 15, 6); // Обратите внимание, что y установлен в 0, а не -16

    // Создание кнопок
    GtkWidget *button_friend = gtk_button_new_with_label("Add friend");
    gtk_widget_set_size_request(button_friend, 115, -1);
    gtk_box_pack_start(GTK_BOX(button_block), button_friend, FALSE, FALSE, 0);
    vendor.helpers.set_classname_and_id(button_friend, "sidebar__bottom_button-block__friend");
    vendor.helpers.add_hover(button_friend);

    GtkWidget *button_group = gtk_button_new_with_label("Create group");
    gtk_widget_set_size_request(button_group, 115, -1);
    gtk_box_pack_start(GTK_BOX(button_block), button_group, FALSE, FALSE, 0);
    vendor.helpers.set_classname_and_id(button_group, "sidebar__bottom_button-block__group");
    vendor.helpers.add_hover(button_group);
    g_signal_connect(button_group, "button-press-event", G_CALLBACK(on_create_group_click), NULL);

    GtkWidget *profile_block = gtk_overlay_new();
    gtk_widget_set_size_request(profile_block, 236, 48);
    vendor.helpers.set_classname_and_id(profile_block, "sidebar__bottom_profile-block");
    gtk_fixed_put(GTK_FIXED(box), profile_block, 12, 50);

    GtkWidget *profile_img = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(profile_img, 48, 48);
//    gtk_fixed_put(GTK_FIXED(profile_block), profile_img, 0, 0);
    gtk_overlay_add_overlay(GTK_OVERLAY(profile_block), profile_img);
    gtk_widget_set_margin_start(profile_img, 0);
    gtk_widget_set_margin_end(profile_img, 216);
    vendor.helpers.set_classname_and_id(profile_img, "sidebar__bottom_profile-block__img");

    GtkWidget *profile_text_block = create_profile_text_block();
    gtk_overlay_add_overlay(GTK_OVERLAY(profile_block), profile_text_block);

    GtkWidget *profile_settings_button = gtk_button_new();
    gtk_widget_set_size_request(profile_settings_button, 20, 20);
    vendor.helpers.set_classname_and_id(profile_settings_button, "sidebar__bottom_profile-block__button");
    GtkWidget *profile_settings_img = gtk_image_new_from_file("resources/images/static/settings.svg");
    gtk_button_set_image(GTK_BUTTON(profile_settings_button), profile_settings_img);
//    gtk_fixed_put(GTK_FIXED(profile_block), profile_settings_button, 216, 12);
    gtk_widget_set_margin_start(profile_settings_button, 208);
    gtk_overlay_add_overlay(GTK_OVERLAY(profile_block), profile_settings_button);
    vendor.helpers.add_hover(profile_settings_button);
    g_signal_connect(profile_settings_button, "button-press-event", G_CALLBACK(on_settings_click), NULL);

    return box;
}
