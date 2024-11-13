#include "../../../inc/header.h"

gboolean set_scroll_to_bottom(gpointer data) {
    GtkAdjustment *vadjustment = GTK_ADJUSTMENT(data);
    gtk_adjustment_set_value(vadjustment, gtk_adjustment_get_upper(vadjustment));
    return FALSE;
}

GtkWidget *chat_init(void) {
    GtkWidget *chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.pages.main_page.chat.chat_box = chat_box;
    vendor.helpers.set_classname_and_id(chat_box, "chat");
    gtk_style_context_add_class(gtk_widget_get_style_context(chat_box), "_right_block");
    gtk_widget_set_halign(chat_box, GTK_ALIGN_FILL);
    gtk_widget_set_valign(chat_box, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(chat_box, TRUE);
    gtk_widget_set_vexpand(chat_box, TRUE);

    GtkWidget *chat_header = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(chat_header, "chat__header");
    gtk_widget_set_size_request(chat_header, -1, 65);
    gtk_box_pack_start(GTK_BOX(chat_box), chat_header, FALSE, FALSE, 0);
    gtk_widget_set_halign(chat_header, GTK_ALIGN_FILL);

    GtkWidget *header_title = gtk_label_new("The very long chat name");
    vendor.helpers.set_classname_and_id(header_title, "chat__header__title");
    gtk_label_set_line_wrap(GTK_LABEL(header_title), TRUE);
    gtk_widget_set_halign(header_title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(chat_header), header_title, TRUE, TRUE, 0);

    GtkWidget *header_info = gtk_label_new("5 days ago");
    vendor.helpers.set_classname_and_id(header_info, "chat__header__info");
    gtk_label_set_line_wrap(GTK_LABEL(header_info), TRUE);
    gtk_widget_set_halign(header_info, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(chat_header), header_info, TRUE, TRUE, 0);

    GtkWidget *scrolled_window = chat_create_scrolled_window();
    gtk_box_pack_start(GTK_BOX(chat_box), scrolled_window, TRUE, TRUE, 0);

    GtkWidget *chat_box_bottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    vendor.helpers.set_classname_and_id(chat_box_bottom, "chat__bottom");
    gtk_widget_set_size_request(chat_box_bottom, -1, 72);
    gtk_box_pack_start(GTK_BOX(chat_box), chat_box_bottom, FALSE, FALSE, 0);

    GtkWidget *message_input = create_message_input();
    gtk_box_pack_start(GTK_BOX(chat_box_bottom), message_input, TRUE, TRUE, 0);

    GtkWidget *message_send = gtk_button_new();
    GtkWidget *message_send_image = gtk_image_new_from_file("resources/images/static/plane.svg");
    vendor.helpers.set_classname_and_id(message_send, "chat__button");
    gtk_container_add(GTK_CONTAINER(message_send), message_send_image);
    vendor.helpers.add_hover(message_send);
    gtk_box_pack_start(GTK_BOX(chat_box_bottom), message_send, FALSE, TRUE, 0);
    gtk_widget_set_size_request(message_send, 34, -1);

    g_idle_add(set_scroll_to_bottom, vendor.pages.main_page.chat.vadjustment);

    return chat_box;
}

t_chat init_chat(void) {
    t_chat chat = {
        .init = chat_init,
        .page = 0,
        .shown_messages = 0,
        .temp_message_counter = 0,
    };
    return chat;
}
