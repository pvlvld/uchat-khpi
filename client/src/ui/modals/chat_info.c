#include "../../../inc/header.h"
#include <gtk/gtk.h>

static void on_invite_clicked(GtkWidget *widget, GdkEventButton *event, t_chat_info *chat_info) {
    (void) widget;
    if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
        int user_count = 0;
        t_users_struct **users = vendor.database.tables.personal_chats_table.get_users(&user_count, chat_info->id);

        vendor.modal.add_users_to_group.show(GTK_WINDOW(vendor.window), user_count, users, chat_info->id);
    }
}

static void on_delete_clicked(GtkWidget *widget, GdkEventButton *event, t_chat_info *chat_info) {
    (void) widget;
    if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
        delete_chat_sidebar(vendor.pages.main_page.sidebar.widget, chat_info->id);
        vendor.modal.chat_info.destroy();
        vendor.database.tables.chats_table.delete_chat_and_related_data(chat_info->id, chat_info->type);
    }
}

static GtkWidget *create_modal_button(const char *label_text, const char *path_to_img) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    vendor.helpers.set_classname_and_id(box, "modal_message__box");

    GtkWidget *image = gtk_image_new_from_file(path_to_img);
    gtk_widget_set_size_request(image, 24, 24);
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 0);

    GtkWidget *label = gtk_label_new(label_text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    vendor.helpers.set_classname_and_id(label, "modal_message__label");

    GtkWidget *event = gtk_event_box_new();
    vendor.helpers.set_classname_and_id(event, "modal_message__event-box");
    gtk_container_add(GTK_CONTAINER(event), box);
    gtk_widget_set_size_request(event, -1, 40);
    vendor.helpers.add_hover(event);

    return event;
}

static void show_modal(GtkWindow *parent, int x, int y, t_chat_info *chat_info) {
    (void) chat_info;
    int height = chat_info->type == PERSONAL ? 58 : 90;
    int width = 143;
    GtkWidget *dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Chat settings");
    vendor.helpers.set_classname_and_id(dialog, "modal__chat-info");
    gtk_style_context_add_class(gtk_widget_get_style_context(dialog), "_modal-base");
    gtk_widget_set_visual(dialog, gdk_screen_get_rgba_visual(gtk_widget_get_screen(dialog)));

    gtk_window_set_default_size(GTK_WINDOW(dialog), width, height);

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);

    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), FALSE);

    gtk_window_move(GTK_WINDOW(dialog), x, y - 40 < 900 ? y - 40 : 840);

    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(content, width, height);
    gtk_container_add(GTK_CONTAINER(dialog), content);

    GtkWidget *delete_event = create_modal_button("Delete", "resources/images/static/delete.svg");
    g_signal_connect(delete_event, "button-press-event", G_CALLBACK(on_delete_clicked), chat_info);

    if (chat_info->type == GROUP) {
        GtkWidget *invite_event = create_modal_button("Add members", "resources/images/static/invite.svg");
        gtk_box_pack_start(GTK_BOX(content), invite_event, FALSE, FALSE, 0);
        g_signal_connect(invite_event, "button-press-event", G_CALLBACK(on_invite_clicked), chat_info);
        gtk_widget_set_margin_top(invite_event, 8);
        gtk_widget_set_margin_bottom(delete_event, 8);
    } else {
        gtk_widget_set_margin_top(delete_event, 8);
    }

    gtk_box_pack_start(GTK_BOX(content), delete_event, FALSE, FALSE, 0);

    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    vendor.modal.chat_info.window = dialog;
}

static void destroy_modal(void) {
    if (vendor.modal.chat_info.window != NULL) {
        gtk_widget_destroy(vendor.modal.chat_info.window);
        vendor.modal.chat_info.window = NULL;
    }
}

t_modal_chat_info init_modal_chat_info(void) {
    t_modal_chat_info chat_info = {
        .window = NULL,
        .show = show_modal,
        .destroy = destroy_modal,
    };

    return chat_info;
}
