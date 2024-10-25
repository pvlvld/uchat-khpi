#include "../../../inc/header.h"
#include <gtk/gtk.h>

static void custom_input_handler(GtkEntry *entry, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(entry);
    g_print("Custom Handler: Typed Text: %s\n", text);

    if (user_data) {
        g_print("Supplemental Data: %s\n", (const gchar *)user_data);
    }

    gtk_entry_set_text(entry, ""); // Очищаем поле ввода
}

static GtkWidget *init_search(void) {
    GtkWidget *entry_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(entry_wrapper, "search-wrapper");
    GtkWidget *entry = vendor.components.input.create("search", "Search", G_CALLBACK(custom_input_handler));

    gtk_box_pack_start(GTK_BOX(entry_wrapper), entry, TRUE, TRUE, 0); // Изменено для GTK3
    return entry_wrapper;
}

GtkWidget *create_chatblock(int avatar_id, const gchar *image_path) {
    GtkWidget *chatblock = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(chatblock, "chatblock");
    gtk_widget_set_size_request(chatblock, 177, -1);
    GtkWidget *avatar_wrapper = vendor.sidebar.create_avatar(avatar_id, image_path);

    GtkWidget *chatblock_text = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(chatblock_text, "chatblock-text");

    GtkWidget *chatblock_text_header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(chatblock_text_header, "chat-header");

    GtkWidget *chat_name = gtk_label_new("Chat name");
    gtk_widget_set_name(chat_name, "chat-name");
    gtk_label_set_lines(GTK_LABEL(chat_name), 1);
    gtk_label_set_ellipsize(GTK_LABEL(chat_name), PANGO_ELLIPSIZE_END);

    GtkWidget *chat_time = gtk_label_new("00:00");
    gtk_widget_set_name(chat_time, "chat-time");

    // Spacer to push chat_time to the right
    GtkWidget *spacer = gtk_label_new(""); // Создаем пустую метку для отступа
    gtk_widget_set_hexpand(spacer, TRUE);  // Позволяем отступу растягиваться горизонтально
    gtk_widget_set_size_request(spacer, 0, -1); // Делаем отступ гибким

    GtkWidget *chat_message = gtk_label_new("The information on who successfully completed the marathon is almost ready, just a little bit left");
    gtk_label_set_line_wrap(GTK_LABEL(chat_message), TRUE);
    gtk_label_set_lines(GTK_LABEL(chat_message), 2);
    gtk_label_set_ellipsize(GTK_LABEL(chat_message), PANGO_ELLIPSIZE_END);
    gtk_widget_set_name(chat_message, "chat-message");

    // Добавление элементов в chatblock
    gtk_box_pack_start(GTK_BOX(chatblock), avatar_wrapper, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(chatblock_text), chatblock_text_header, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(chatblock_text), chat_message, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(chatblock_text_header), chat_name, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(chatblock_text_header), spacer, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(chatblock_text_header), chat_time, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(chatblock), chatblock_text, TRUE, TRUE, 0);

    return chatblock;
}

GtkWidget *sidebar_init(void) {
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(sidebar, "sidebar");
    gtk_widget_set_size_request(sidebar, 260, -1);
    gtk_widget_set_hexpand(sidebar, FALSE);

    GtkWidget *search = init_search();
    gtk_box_pack_start(GTK_BOX(sidebar), search, FALSE, FALSE, 0); // Изменено для GTK3

    // Блок для автоматического скролла
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL); // Создаем новое окно прокрутки с NULL для параметров
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkWidget *stretchable_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(stretchable_box, TRUE); // Позволяем растягиваемому блоку занимать всю доступную высоту

    // Добавляем растягиваемый блок в окно прокрутки
    gtk_container_add(GTK_CONTAINER(scrolled_window), stretchable_box); // Исправлено на gtk_container_add

    gtk_box_pack_start(GTK_BOX(sidebar), scrolled_window, TRUE, TRUE, 0); // Изменено для GTK3

    // Заполнение растягиваемого блока элементами
    for (int i = 0; i < 10; i++) {
        GtkWidget *avatar = create_chatblock(i, "person_img.jpg");
        gtk_box_pack_start(GTK_BOX(stretchable_box), avatar, FALSE, FALSE, 0); // Используем gtk_box_pack_start для добавления
    }
    // Установка свойства vexpand для scrolled_window
    gtk_widget_set_vexpand(scrolled_window, TRUE); // Это позволяет окну прокрутки занимать доступное пространство

    // Фиксированный блок внизу
    GtkWidget *fixed_height_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(fixed_height_box, -1, 80);  // Высота 80px
    gtk_widget_set_name(fixed_height_box, "fixed-height-box");

    GtkWidget *fixed_label = gtk_label_new("Fixed Height Box");
    gtk_box_pack_start(GTK_BOX(fixed_height_box), fixed_label, FALSE, FALSE, 0); // Изменено для GTK3

    gtk_box_pack_start(GTK_BOX(sidebar), fixed_height_box, FALSE, FALSE, 0); // Изменено для GTK3

    return sidebar;
}

t_sidebar init_sidebar(void) {
    t_sidebar sidebar = {
        .init = sidebar_init,
        .create_avatar = sidebar_create_avatar
    };
    return sidebar;
}
