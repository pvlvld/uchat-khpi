#include "../inc/header.h"
#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *sidebar;
    GtkWidget *right_box;

    // Создание нового окна
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "ShadowTalk");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);

    // Создание горизонтального контейнера
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    // Создание левой панели с прокруткой и правой панели
    sidebar = init_sidebar();
    right_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Создание и добавление метки в правую панель
    GtkWidget *label = gtk_label_new("Select a chat to start messaging");  // Создание метки

    // Установка свойств для метки
    gtk_widget_set_halign(label, GTK_ALIGN_FILL);  // Заполнение по горизонтали
    gtk_widget_set_valign(label, GTK_ALIGN_FILL);  // Заполнение по вертикали

    // Установка размеров метки
    gtk_widget_set_hexpand(label, TRUE);  // Разрешение расширения по горизонтали
    gtk_widget_set_vexpand(label, TRUE);  // Разрешение расширения по вертикали

    // Добавление метки в правую панель
    gtk_box_append(GTK_BOX(right_box), label);  // Добавление метки в правую панель

    // Добавление панелей в основной контейнер
    gtk_box_append(GTK_BOX(box), sidebar);
    gtk_box_append(GTK_BOX(box), right_box);

    // Применение цветового стиля через CSS
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "resources/styles/style.css"); // Загрузка из файла

    // Применение CSS к основному контейнеру
    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Установка идентификаторов для панелей
    gtk_widget_set_name(right_box, "right-box");

    // Добавление боксов в окно
    gtk_window_set_child(GTK_WINDOW(window), box);

    // Отображение окна
    gtk_window_present(GTK_WINDOW(window));  // Изменено: используем gtk_window_present

    // Освобождение ресурсов
    g_object_unref(css_provider);
    if (user_data) return; // Plug
}

int main(int argc, char **argv) {
    init_vendor(&vendor);
    GtkApplication *app;
    int status;

    // Создание нового приложения GTK
    app = gtk_application_new("org.example.GtkApplication", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Запуск приложения
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
