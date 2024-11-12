#include "../../../inc/header.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>

static void custom_input_handler(GtkEntry *entry, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(entry);
    g_print("Custom Handler: Typed Text: %s\n", text);

    if (user_data) {
        g_print("Supplemental Data: %s\n", (const gchar *)user_data);
    }

    gtk_entry_set_text(entry, "");
}

static GtkWidget *init_search(void) {
    GtkWidget *entry_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(entry_wrapper, "sidebar__search_wrapper");
    GtkWidget *entry = vendor.components.input.create("sidebar__search", "Search", G_CALLBACK(custom_input_handler));

    gtk_box_pack_start(GTK_BOX(entry_wrapper), entry, TRUE, TRUE, 0);
    return entry_wrapper;
}

static void swap_sidebar(GtkWidget *widget, ssize_t index) {
    GtkWidget *stretchable_box = g_object_get_data(G_OBJECT(widget), "stretchable_box");

    GList *children = gtk_container_get_children(GTK_CONTAINER(stretchable_box));

    if (children != NULL) {
        for (ssize_t i = 0; i < g_list_length(children); i++) {
            GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(children, i));
            t_chat_info *chat_info = g_object_get_data(G_OBJECT(target_child), "chat_info");
            if (chat_info->id != index) continue;
            GtkWidget *new_child = vendor.pages.main_page.sidebar.create_chatblock(chat_info);

            if (vendor.active_chat.chat_sidebar_widget == target_child) {
                vendor.active_chat.chat_sidebar_widget = new_child;
                gtk_style_context_add_class(gtk_widget_get_style_context(new_child), "active");
            }
            if (vendor.hover_chat.chat_sidebar_widget == target_child) {
                vendor.hover_chat.chat_sidebar_widget = new_child;
                gtk_style_context_add_class(gtk_widget_get_style_context(new_child), "hover");
            }
            gtk_widget_destroy(target_child);

            gtk_box_pack_end(GTK_BOX(stretchable_box), new_child, TRUE, FALSE, 0);
            gtk_widget_show(new_child);

            gtk_widget_show_all(stretchable_box);
        }
        g_list_free(children);
    }
}

static gboolean key_press_handler(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    (void) widget;
    (void) user_data;

    if (event->keyval == GDK_KEY_Escape) {
//        g_print("Esc pressed\n");
        if (vendor.active_chat.chat_sidebar_widget != NULL) {
            gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.active_chat.chat_sidebar_widget), "active");
            vendor.active_chat.chat_sidebar_widget = NULL;
            vendor.active_chat.chat = NULL;
        }
        return TRUE;
    }

    if (event->keyval == GDK_KEY_space) {
		if (vendor.active_chat.chat_sidebar_widget != NULL) {
        	g_print("id: %d\nname: %s\nlast_message: %s\nsender_name: %s\ntype: %d\n"
					"path_to_logo: %s\nunreaded_messages: %d\ntimestamp: %s\n",
                          vendor.active_chat.chat->id,
                          vendor.active_chat.chat->name,
                          vendor.active_chat.chat->last_message,
                          vendor.active_chat.chat->sender_name,
                          vendor.active_chat.chat->type,
                          vendor.active_chat.chat->path_to_logo,
                          vendor.active_chat.chat->unreaded_messages,
                          format_timestamp(vendor.active_chat.chat->timestamp));
		}
        return TRUE;
    }

    if ((event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_KEY_w)) {
        vendor.database.tables.chats_table.add_chat(3, "personal");
        t_chats_struct *chat = vendor.database.tables.chats_table.get_chat_by_id(3);
        if (chat != NULL) {
            printf("Chat ID: %d\n", chat->chat_id);
            printf("Chat Type: %d\n", chat->chat_type);
            printf("Created At: %s", asctime(&chat->created_at));

            free(chat);
        } else {
            printf("Chat with ID %d not found.\n", 123);
        }
    }

    if ((event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_KEY_e)) {
        int total_messages = 0;
        int page = 1;
        int message_count = 10;

        t_messages_struct *messages = vendor.database.tables.messages_table.get_messages_by_chat_id(1, "timestamp", "DESC", message_count, page, &total_messages);
        if (messages != NULL) {
            printf("Total messages: %d\n", total_messages);
            for (int i = 0; i < message_count; i++) {
                if ((i + 1) * page > total_messages) break;
                printf("Message ID: %d\n", messages[i].message_id);
                printf("Sender: %d\n", messages[i].sender_struct->user_id);
                printf("Text: %s\n", messages[i].message_text);
            }

            vendor.database.tables.messages_table.free_struct(messages);
        }

        ssize_t index = rand() % 12;
        g_print("Element with id %zd updated!\n", index);
        swap_sidebar(widget, index);
        vendor.helpers.show_notification("New notification", "New message");
        return TRUE;
    }

    return FALSE;
}

static void on_widget_destroy(GtkWidget *widget, gpointer user_data) {
    (void) user_data;
    (void) widget;
    vendor.active_chat.chat_sidebar_widget = NULL;
    vendor.hover_chat.chat_sidebar_widget = NULL;
}

GtkWidget *sidebar_init(void) {
    (void)key_press_handler;
    (void)on_widget_destroy;

    // Создаем основной контейнер для сайдбара
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(sidebar, "sidebar");
    gtk_widget_set_size_request(sidebar, 260, -1);
    gtk_widget_set_hexpand(sidebar, FALSE);

    // Инициализируем поиск
    GtkWidget *search = init_search();
    gtk_box_pack_start(GTK_BOX(sidebar), search, FALSE, FALSE, 0);

    // Создаем окно с прокруткой для чатов
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    vendor.helpers.set_classname_and_id(scrolled_window, "sidebar__scrolled-window");

    // Создаем контейнер для чатов (stretchable_box)
    GtkWidget *stretchable_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(stretchable_box, TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), stretchable_box);

    // Сохраняем stretchable_box в sidebar для использования в swap_sidebar
    g_object_set_data(G_OBJECT(sidebar), "stretchable_box", stretchable_box);

    // Добавляем окно с прокруткой в сайдбар
    gtk_box_pack_start(GTK_BOX(sidebar), scrolled_window, TRUE, TRUE, 0);

    // Получаем данные чатов из БД
    t_chat_info **chats_info = parse_chats_info();
    if (!chats_info) {
        printf("[ERROR] Не удалось получить информацию о чатах.\n");
        return sidebar;
    }

    // Создаем блоки чатов и добавляем их в stretchable_box
    size_t i = 0;
    while (chats_info[i] != NULL) {
        printf("[DEBUG] Создание chatblock для чата с ID: %d\n", chats_info[i]->id);
        GtkWidget *chatblock = vendor.pages.main_page.sidebar.create_chatblock(chats_info[i]);
        if (!chatblock) {
            printf("[ERROR] Не удалось создать блок для чата с ID: %d\n", chats_info[i]->id);
            continue;
        }

        g_object_set_data(G_OBJECT(chatblock), "chat_info", chats_info[i]);
        gtk_box_pack_end(GTK_BOX(stretchable_box), chatblock, FALSE, FALSE, 0);
        gtk_widget_show(chatblock);
        i++;
    }

    // Освобождаем память, если чаты были получены
    free_chats_info(chats_info);

    // Возвращаем сайдбар
    return sidebar;
}

t_sidebar init_sidebar(void) {
    t_sidebar sidebar = {
        .init = sidebar_init,
        .create_chatblock = sidebar_create_chatblock,
        .create_avatar = sidebar_create_avatar,
        .create_bottom = sidebar_create_bottom,
    };
    return sidebar;
}
