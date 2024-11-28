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

static void on_file_open(GtkWidget *widget, gpointer user_data) {
    (void)widget;
    (void)user_data;

    GtkWindow *dialog_window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

    gtk_window_set_default_size(dialog_window, 600, 303);

    gtk_window_set_resizable(dialog_window, TRUE);

    int parent_width, parent_height;
    gtk_window_get_size(GTK_WINDOW(vendor.window), &parent_width, &parent_height);
    int parent_x, parent_y;
    gtk_window_get_position(GTK_WINDOW(vendor.window), &parent_x, &parent_y);

    gtk_window_move(dialog_window,
                    parent_x + (parent_width / 2) - 300,
                    parent_y + (parent_height / 2) - 150);

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File",
                                                    dialog_window,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    gint res = gtk_dialog_run(GTK_DIALOG(dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        char *file_path = gtk_file_chooser_get_filename(chooser);

        if (file_path) {
            const char *valid_extensions[] = {"jpg", "jpeg", "png", NULL};
            const char *file_extension = g_strrstr(file_path, ".");
            gboolean is_valid_image = FALSE;

            for (int i = 0; valid_extensions[i] != NULL; i++) {
                if (file_extension && g_str_has_suffix(file_extension, valid_extensions[i])) {
                    is_valid_image = TRUE;
                    break;
                }
            }

            if (is_valid_image) {
                GDateTime *now = g_date_time_new_now_utc();
                gchar *timestamp = g_date_time_format(now, "%Y%m%d%H%M%S");
                g_date_time_unref(now);

                const char *basename = g_path_get_basename(file_path);
                gchar *new_filename = g_strdup_printf("%s_%s%s", timestamp, basename, file_extension);

                const char *dest_dir = "resources/images/user_uploads";
                gchar *destination_path = g_build_filename(dest_dir, new_filename, NULL);

                g_mkdir_with_parents(dest_dir, 0755);

                GFile *src_file = g_file_new_for_path(file_path);
                GFile *dst_file = g_file_new_for_path(destination_path);

                // Копируем файл
                if (g_file_test(file_path, G_FILE_TEST_EXISTS)) {
                    if (g_file_copy(src_file, dst_file, G_FILE_COPY_OVERWRITE, NULL, NULL, NULL, NULL)) {
                        g_print("File successfully copied to: %s\n", destination_path);
                    } else {
                        g_warning("Failed to copy file to: %s\n", destination_path);
                    }
                }

                g_object_unref(src_file);
                g_object_unref(dst_file);

                g_free(destination_path);
                g_free(new_filename);
            } else {
                g_warning("Selected file is not a valid image.\n");
            }

            g_free(file_path);
        } else {
            g_warning("No file selected.");
        }
    }

    gtk_widget_destroy(dialog);
    gtk_widget_destroy(GTK_WIDGET(dialog_window));
}

static void show_modal(GtkWindow *parent, t_message_info_struct *message_info_struct, int x, int y, const char *text, gboolean is_full) {
    int width = 201;
    int height = 176;
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

    g_print("x: %d y: %d\n", x, y);

//    gtk_window_move(GTK_WINDOW(dialog), x + width < window_width + 54 ? x : window_width - width + 54, y - 40 < 900 ? y - 40 : 840);
    gtk_window_move(GTK_WINDOW(dialog), x, y - 40 < 940 ? y - 40 : 880);

    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(content, width, height);
    gtk_container_add(GTK_CONTAINER(dialog), content);

    GtkWidget *add_reaction_event = create_modal_button("Add Reaction", "resources/images/static/add_reaction.svg");
    GtkWidget *edit_event = create_modal_button("Edit", "resources/images/static/edit.svg");
    GtkWidget *copy_event = create_modal_button(is_full ? "Copy text" : "Copy selected text", "resources/images/static/copy.svg");
    GtkWidget *delete_event = create_modal_button("Delete", "resources/images/static/delete.svg");

    gtk_widget_set_margin_top(add_reaction_event, 8);
    gtk_widget_set_margin_bottom(delete_event, 8);

    g_signal_connect(edit_event, "button-press-event", G_CALLBACK(on_file_open), vendor.window);
    g_signal_connect(copy_event, "button-press-event", G_CALLBACK(on_copy_clicked), (gpointer)text);
    g_signal_connect(delete_event, "button-press-event", G_CALLBACK(on_delete_clicked), message_info_struct);

    gtk_box_pack_start(GTK_BOX(content), add_reaction_event, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content), edit_event, FALSE, FALSE, 0);
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
    };

    return message_info;
}
