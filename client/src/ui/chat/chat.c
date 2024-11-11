#include "../../../inc/header.h"

void on_size_allocate(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data) {
    (void)widget;
    (void)user_data;
    gtk_widget_set_margin_bottom(widget, allocation->height - 18);
}

void on_size_allocate2(GtkWidget *widget, GdkRectangle *allocation, gpointer user_data) {
    (void)widget;
    (void)user_data;
    (void)allocation;
    static int is_called = 0;

    if (allocation->height > 100 && is_called == 0) {
        GtkAdjustment *v_adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(widget));
        gtk_adjustment_set_value(v_adjustment, gtk_adjustment_get_upper(v_adjustment) - gtk_adjustment_get_page_size(v_adjustment));
        is_called++;
    }
}

GtkWidget *chat_init(void) {
    GtkWidget *chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
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

//    GtkWidget *chat_chats_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
//    vendor.helpers.set_classname_and_id(chat_chats_box, "chat__chats-box");
//    gtk_widget_set_size_request(chat_chats_box, -1, -1);
//    gtk_box_pack_start(GTK_BOX(chat_box), chat_chats_box, TRUE, TRUE, 0);
//    gtk_widget_set_valign(chat_chats_box, GTK_ALIGN_END);
////    GtkWidget *message_block = gtk_fixed_new();
//
//    for (unsigned int i = 0; i < 2; i++) {
//        GtkWidget *message = gtk_fixed_new();
//        vendor.helpers.set_classname_and_id(message, "chat__message");
//        GtkWidget *message_text = gtk_label_new("Test1 Test2 Test3 Test4 Test5 Test6 Test7 Test8 Test9 Test10 Test11 Test12 Test13 Test14 Test15");
//        gtk_widget_set_size_request(message_text, 500, -1);
//        gtk_label_set_line_wrap(GTK_LABEL(message_text), TRUE);
//        vendor.helpers.set_classname_and_id(message_text, "chat__message__text");
//        gtk_fixed_put(GTK_FIXED(message), message_text, 0, 0);
//        gtk_box_pack_start(GTK_BOX(chat_chats_box), message, FALSE, FALSE, 4);
//        g_signal_connect(message_text, "size-allocate", G_CALLBACK(on_size_allocate), NULL);
//    }

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    vendor.helpers.set_classname_and_id(scrolled_window, "chat__scrolled-window");
    gtk_box_pack_start(GTK_BOX(chat_box), scrolled_window, TRUE, TRUE, 0);

    GtkWidget *stretchable_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(stretchable_box, TRUE);
    gtk_widget_set_valign(stretchable_box, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(scrolled_window), stretchable_box);

    for (unsigned int i = 0; i < 20; i++) {
        GtkWidget *message_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_widget_set_hexpand(stretchable_box, TRUE);
        gtk_widget_set_halign(message_wrapper, rand() % 2 == 0 ? GTK_ALIGN_END : GTK_ALIGN_START);
        vendor.helpers.set_classname_and_id(message_wrapper, "chat__message__wrapper");
        gtk_box_pack_start(GTK_BOX(stretchable_box), message_wrapper, FALSE, FALSE, 0);

        GtkWidget *message = gtk_overlay_new();
        gtk_widget_set_hexpand(message, FALSE);
        gtk_widget_set_size_request(message, 500, 36);
        vendor.helpers.set_classname_and_id(message, "chat__message");
        gtk_box_pack_start(GTK_BOX(message_wrapper), message, FALSE, FALSE, 0);


        GtkWidget *message_text = gtk_label_new("Test1 Test2 Test3 Test4 Test5 Test6 Test7 Test8 Test9 Test10 Test11 Test12 Test13 Test14 Test15 Test1 Test2 Test3 Test4 Test5 Test6 Test7 Test8 Test9 Test10 Test11 Test12 Test13 Test14 Test15 ");
        gtk_widget_set_size_request(message_text, 500, -1);
        gtk_label_set_line_wrap(GTK_LABEL(message_text), TRUE);
        vendor.helpers.set_classname_and_id(message_text, "chat__message__text");
        g_signal_connect(message_text, "size-allocate", G_CALLBACK(on_size_allocate), NULL);
        gtk_overlay_add_overlay(GTK_OVERLAY(message), message_text);
    }
    g_signal_connect(scrolled_window, "size-allocate", G_CALLBACK(on_size_allocate2), NULL);

    GtkWidget *chat_box_bottom = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(chat_box_bottom, "chat__bottom");
    gtk_widget_set_size_request(chat_box_bottom, -1, 72);
    gtk_box_pack_start(GTK_BOX(chat_box), chat_box_bottom, FALSE, FALSE, 0);

    return chat_box;
}

t_chat init_chat(void) {
    t_chat chat = {
        .init = chat_init,
    };
    return chat;
}
