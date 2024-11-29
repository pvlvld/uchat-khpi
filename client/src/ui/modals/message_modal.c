#include "../../../inc/header.h"
#include <gtk/gtk.h>

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

static void on_copy_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    (void) widget;
    char *text = (char *)user_data;
    if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
        GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
        gtk_clipboard_set_text(clipboard, text, -1);

        vendor.popup.add_message("Added to clipboard");
        vendor.modal.message_info.destroy();
        g_free(text);
        vendor.modal.message_info.destroy();
    }
}

static void on_edit_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    (void) widget;
    (void) user_data;
    if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
        t_message_info_modal *message_info = (t_message_info_modal *)user_data;
        vendor.modal.message_info.edit_modal.show(GTK_WINDOW(vendor.window), message_info);
    }
}

static void on_delete_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    (void) widget;
    if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
        t_message_info_struct *message_info_struct = (t_message_info_struct *)user_data;

        GtkWidget *stretchable_box = message_info_struct->is_new ? vendor.pages.main_page.chat.stretchable_box_new_messages : vendor.pages.main_page.chat.stretchable_box_old_messages;

        int height = gtk_widget_get_allocated_height(message_info_struct->widget);
        int content_box_height = gtk_widget_get_allocated_height(stretchable_box);

        gtk_widget_set_size_request(stretchable_box, -1, content_box_height - height);
        gtk_widget_destroy(message_info_struct->widget);

        vendor.modal.message_info.destroy();
        vendor.database.tables.messages_table.delete_message(message_info_struct->message_id, message_info_struct->chat_id);

    }
}

static void show_modal(GtkWindow *parent, t_message_info_struct *message_info_struct, int x, int y, const char *text, gboolean is_full) {
    int width = 201;
//    int height = 176;
    int height = 126;
    int window_width;
    gtk_window_get_size(GTK_WINDOW(vendor.window), &window_width, NULL);
    GtkWidget *dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Chat settings");
    vendor.helpers.set_classname_and_id(dialog, "modal_message");
    gtk_style_context_add_class(gtk_widget_get_style_context(dialog), "_modal-base");
    gtk_widget_set_visual(dialog, gdk_screen_get_rgba_visual(gtk_widget_get_screen(dialog)));

    gtk_window_set_default_size(GTK_WINDOW(dialog), width, height);

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(dialog), FALSE);

    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), FALSE);

//    gtk_window_move(GTK_WINDOW(dialog), x + width < window_width + 54 ? x : window_width - width + 54, y - 40 < 900 ? y - 40 : 840);
    gtk_window_move(GTK_WINDOW(dialog), x, y - 40 < 940 ? y - 40 : 880);

    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(content, width, height);
    gtk_container_add(GTK_CONTAINER(dialog), content);

    GtkWidget *add_reaction_event = create_modal_button("Add Reaction", "resources/images/static/add_reaction.svg");

    GtkWidget *copy_event = create_modal_button(is_full ? "Copy text" : "Copy selected text", "resources/images/static/copy.svg");
    GtkWidget *delete_event = create_modal_button("Delete", "resources/images/static/delete.svg");

    gtk_widget_set_margin_top(add_reaction_event, 8);
    gtk_widget_set_margin_bottom(delete_event, 8);
	t_message_info_modal *message_info = malloc(sizeof(t_message_info_modal *));

    g_signal_connect(copy_event, "button-press-event", G_CALLBACK(on_copy_clicked), (gpointer)text);
    g_signal_connect(delete_event, "button-press-event", G_CALLBACK(on_delete_clicked), message_info_struct);

    gtk_box_pack_start(GTK_BOX(content), add_reaction_event, FALSE, FALSE, 0);
    if (vendor.current_user.user_id == message_info_struct->sender_id) {
        GtkWidget *edit_event = create_modal_button("Edit", "resources/images/static/edit.svg");
        message_info->text = vendor.helpers.strdup(text);
        message_info->info = message_info_struct;
        g_signal_connect(edit_event, "button-press-event", G_CALLBACK(on_edit_clicked), message_info);
        gtk_box_pack_start(GTK_BOX(content), edit_event, FALSE, FALSE, 0);
        gtk_window_set_default_size(GTK_WINDOW(dialog), width, height + 30);
    }

    gtk_box_pack_start(GTK_BOX(content), copy_event, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content), delete_event, FALSE, FALSE, 0);

    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    vendor.modal.message_info.window = dialog;
}

static void destroy_modal(void) {
    if (vendor.modal.message_info.window != NULL) {
        gtk_widget_destroy(vendor.modal.message_info.window);
        vendor.modal.message_info.window = NULL;
    }
}

t_modal_message_info init_modal_message_info(void) {
    t_modal_message_info message_info = {
        .window = NULL,
        .show = show_modal,
        .destroy = destroy_modal,
        .edit_modal = init_edit_modal(),
    };

    return message_info;
}
