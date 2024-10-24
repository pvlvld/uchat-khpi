#include "../../../inc/header.h"
#include <gtk/gtk.h>

static void custom_input_handler(GtkEntry *entry, gpointer user_data) {
    const gchar *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    g_print("Custom Handler: Typed Text: %s\n", text);

    if (user_data) {
        g_print("Supplemental Data: %s\n", (const gchar *)user_data);
    }

    gtk_editable_set_text(GTK_EDITABLE(entry), "");
}

GtkWidget *create_scrolled_left_box(void) {
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);

    GtkWidget *sidebar = init_sidebar();

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), sidebar);

    return scrolled_window;
}

static GtkWidget *init_search(void) {
    GtkWidget *entry_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(entry_wrapper, "search-wrapper");
    GtkWidget *entry = vendor.components.input.create("search", "Search", G_CALLBACK(custom_input_handler));

    gtk_box_append(GTK_BOX(entry_wrapper), entry);
    return entry_wrapper;
}

GtkWidget *create_avatar_with_wrapper(int avatar_id, const gchar *image_path) {
    // Создаем контейнер для аватара
    GtkWidget *avatar_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(avatar_wrapper, "avatar-wrapper");  // Идентификатор для CSS

    // Создаем красный блок (представляющий аватар)
    GtkWidget *avatar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(avatar, "avatar");  // Идентификатор для CSS

    gchar avatar_class_str[64];
    g_snprintf(avatar_class_str, sizeof(avatar_class_str), "avatar-class-%d", avatar_id);
    gtk_widget_set_name(avatar, "avatar");
    gtk_widget_add_css_class(avatar, avatar_class_str);  // Идентификатор для CSS
    gtk_widget_set_size_request(avatar, 48, 48);  // Устанавливаем размер блока

    gchar css[512];
    g_snprintf(css, sizeof(css), ".%s { background-image: url('file://%s');}", avatar_class_str, image_path);

    // Устанавливаем выравнивание для аватара
    gtk_widget_set_halign(avatar_wrapper, GTK_ALIGN_CENTER); // Начальное выравнивание по горизонтали
    gtk_widget_set_valign(avatar_wrapper, GTK_ALIGN_CENTER); // Начальное выравнивание по вертикали

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );


    // Добавляем красный блок аватара в контейнер
    gtk_box_append(GTK_BOX(avatar_wrapper), avatar);

    return avatar_wrapper;
}

GtkWidget *init_sidebar(void) {
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(sidebar, "sidebar");
    gtk_widget_set_size_request(sidebar, 260, -1);  // 260px ширина, высота авто

    GtkWidget *search = init_search();
    gtk_box_append(GTK_BOX(sidebar), search);

    // Блок для автоматического скролла
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkWidget *stretchable_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(stretchable_box, TRUE); // Позволяем растягиваемому блоку занимать всю доступную высоту

    // Добавляем растягиваемый блок в окно прокрутки
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), stretchable_box);
    gtk_box_append(GTK_BOX(sidebar), scrolled_window);

    // Заполнение растягиваемого блока элементами
    for (int i = 0; i < 10; i++) {
        GtkWidget *avatar = create_avatar_with_wrapper(i, "/home/roman/Desktop/gitlab/uchat/client/resources/images/static/person_img.jpg");
        gtk_box_append(GTK_BOX(stretchable_box), avatar);
    }

    // Установка свойства vexpand для scrolled_window
    gtk_widget_set_vexpand(scrolled_window, TRUE); // Это позволяет окну прокрутки занимать доступное пространство

    // Фиксированный блок внизу
    GtkWidget *fixed_height_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(fixed_height_box, -1, 80);  // Высота 330px
    gtk_widget_set_name(fixed_height_box, "fixed-height-box");

    GtkWidget *fixed_label = gtk_label_new("Fixed Height Box");
    gtk_box_append(GTK_BOX(fixed_height_box), fixed_label);

    gtk_box_append(GTK_BOX(sidebar), fixed_height_box);

    return sidebar;
}
