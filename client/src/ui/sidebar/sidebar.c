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

void message_receipt(GtkWidget *widget, t_api_message_struct *message) {
    GtkWidget *stretchable_box = g_object_get_data(G_OBJECT(widget), "stretchable_box");

    GList *children = gtk_container_get_children(GTK_CONTAINER(stretchable_box));

    if (children != NULL) {
        for (ssize_t i = 0; i < g_list_length(children); i++) {
            GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(children, i));
            t_chat_info *chat_info = g_object_get_data(G_OBJECT(target_child), "chat_info");
            if (chat_info->id != (unsigned int)message->chat_id) continue;
            gtk_box_reorder_child(GTK_BOX(stretchable_box), target_child, 0);

            t_messages_struct *message_struct = vendor.database.tables.messages_table.add_message(message->message_id,
                        message->chat_id, message->sender_id, message->message_encrypted, NULL, message->timestamp);

            chat_info->last_message = message_struct;

            update_chatblock(target_child, chat_info, 1);

            if (chat_info->unreaded_messages < 1) chat_info->unreaded_messages = 0;

            if (vendor.active_chat.chat_sidebar_widget != NULL) {
                if (vendor.active_chat.chat->id == chat_info->id) {
                    g_print("message_struct == NULL: %d\n", message_struct == NULL);
                    add_chat_message(message_struct, 1);
                } else {
                    chat_info->unreaded_messages++;
                    vendor.helpers.show_notification("New notification", "New message");
                    vendor.popup.add_message("New message");
                }
            } else {
                chat_info->unreaded_messages++;
                vendor.helpers.show_notification("New notification", "New message");
                vendor.popup.add_message("New message");
            }

            break;
        }
        g_list_free(children);
    }
}

void delete_chat_sidebar(GtkWidget *widget, ssize_t index) {
    GtkWidget *stretchable_box = g_object_get_data(G_OBJECT(widget), "stretchable_box");

    GList *children = gtk_container_get_children(GTK_CONTAINER(stretchable_box));

    if (children != NULL) {
        for (ssize_t i = 0; i < g_list_length(children); i++) {
            GtkWidget *target_child = GTK_WIDGET(g_list_nth_data(children, i));
            t_chat_info *chat_info = g_object_get_data(G_OBJECT(target_child), "chat_info");
            if (chat_info->id != index) continue;

            if (vendor.active_chat.chat_sidebar_widget == target_child) {
                gtk_style_context_remove_class(gtk_widget_get_style_context(target_child), "active");
                vendor.active_chat.chat_sidebar_widget = NULL;
                vendor.pages.main_page.chat.change_chat();
                vendor.active_chat.chat = NULL;
            }

            if (vendor.hover_chat.chat_sidebar_widget == target_child) {
                gtk_style_context_remove_class(gtk_widget_get_style_context(target_child), "hover");
                vendor.hover_chat.chat_sidebar_widget = NULL;
                vendor.hover_chat.chat = NULL;
            }

            gtk_widget_destroy(GTK_WIDGET(target_child));

            gtk_widget_show_all(stretchable_box);
            break;
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
            vendor.pages.main_page.chat.change_chat();
            vendor.active_chat.chat = NULL;
        }
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

void activate_chatblock_by_id(int chat_id) {
    GtkWidget *stretchable_box = vendor.sidebar.stretchable_box;

    if (!stretchable_box) {
        if (vendor.debug_mode >= 1) printf("[ERROR] stretchable_box not found in sidebar.\n");
        return;
    }

    GList *children = gtk_container_get_children(GTK_CONTAINER(stretchable_box));

    if (!children) {
        if (vendor.debug_mode >= 1) printf("[INFO] No chatblocks found.\n");
        return;
    }

    for (GList *iter = children; iter != NULL; iter = iter->next) {
        GtkWidget *chatblock = GTK_WIDGET(iter->data);
        t_chat_info *chat_info = g_object_get_data(G_OBJECT(chatblock), "chat_info");

        if (chat_info && chat_info->id == (unsigned) chat_id) {
            vendor.active_chat.chat = chat_info;

            if (vendor.active_chat.chat_sidebar_widget != NULL) {
                gtk_style_context_remove_class(gtk_widget_get_style_context(vendor.active_chat.chat_sidebar_widget), "active");
            }

            vendor.active_chat.chat_sidebar_widget = chatblock;
            vendor.pages.main_page.chat.change_chat();

            vendor.active_chat.chat->unreaded_messages = 0;

            update_chatblock(chatblock, vendor.active_chat.chat, 0);

            gtk_style_context_add_class(gtk_widget_get_style_context(vendor.active_chat.chat_sidebar_widget), "active");

            g_list_free(children);
            return;
        }
    }

    if (vendor.debug_mode >= 1) printf("[INFO] Chatblock with ID %d not found.\n", chat_id);
    g_list_free(children);
}

GtkWidget *sidebar_init(void) {
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(sidebar, "sidebar");
    gtk_widget_set_size_request(sidebar, 260, -1);
    gtk_widget_set_hexpand(sidebar, FALSE);

    GtkWidget *search = init_search();
    gtk_box_pack_start(GTK_BOX(sidebar), search, FALSE, FALSE, 0);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    vendor.helpers.set_classname_and_id(scrolled_window, "sidebar__scrolled-window");

    GtkWidget *stretchable_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(stretchable_box, TRUE);
    vendor.sidebar.stretchable_box = stretchable_box;

    gtk_container_add(GTK_CONTAINER(scrolled_window), stretchable_box);

    gtk_box_pack_start(GTK_BOX(sidebar), scrolled_window, TRUE, TRUE, 0);

    t_chat_info **chats_info = parse_chats_info();
    if (!chats_info) {
        if (vendor.debug_mode >= 1) printf("[INFO] Failed to retrieve chat information.\n");
    } else {
        size_t i = 0;
        while (chats_info[i] != NULL) {
            if (vendor.debug_mode == 1) printf("[DEBUG] Creating a chatblock for chat with ID: %d\n", chats_info[i]->id);
            GtkWidget *chatblock = vendor.pages.main_page.sidebar.create_chatblock(chats_info[i]);
            if (!chatblock) {
                printf("[ERROR] Failed to create a block for chat with ID: %d\n", chats_info[i]->id);
                continue;
            }

            g_object_set_data(G_OBJECT(chatblock), "chat_info", chats_info[i]);

            gtk_box_pack_start(GTK_BOX(stretchable_box), chatblock, FALSE, FALSE, 0);
            gtk_widget_show(chatblock);
            i++;
        }
    }

    g_object_set_data(G_OBJECT(sidebar), "stretchable_box", stretchable_box);

    gtk_widget_set_vexpand(scrolled_window, TRUE);

    GtkWidget *bottom_block = vendor.pages.main_page.sidebar.create_bottom();

    gtk_box_pack_end(GTK_BOX(sidebar), bottom_block, FALSE, FALSE, 0);

    g_signal_connect(vendor.pages.main_page.main_page, "key-press-event", G_CALLBACK(key_press_handler), NULL);
    g_signal_connect(sidebar, "destroy", G_CALLBACK(on_widget_destroy), NULL);

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
