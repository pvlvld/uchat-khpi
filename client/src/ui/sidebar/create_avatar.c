#include "../../../inc/header.h"
#include <gtk/gtk.h>

GtkWidget *sidebar_create_avatar(t_chat_info *chat_info) {
    GtkWidget *fixed_container = gtk_fixed_new();
    gtk_widget_set_size_request(fixed_container, 55, 48);
    vendor.helpers.set_classname_and_id(fixed_container, "chatblock__avatar");

    GtkWidget *avatar = vendor.helpers.create_avatar(chat_info->path_to_logo, 48, 48);
    // chat_info->path_to_logo
    vendor.helpers.set_classname_and_id(avatar, "chatblock__avatar_image");

    gtk_fixed_put(GTK_FIXED(fixed_container), avatar, 0, 0);

    GtkWidget *group_type = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(group_type, "chatblock__avatar_group-type");
    gtk_style_context_add_class(gtk_widget_get_style_context(group_type), chat_info->type == 0 ? "_single" : "_group");

    gtk_widget_set_size_request(group_type, 14, 14);
    gtk_fixed_put(GTK_FIXED(fixed_container), group_type, 0, 0);
    if (chat_info->unreaded_messages != 0) {
        char unreaded_messages[20];
        sprintf(unreaded_messages, "%d", chat_info->unreaded_messages);
        GtkWidget *number_of_messages = gtk_label_new(unreaded_messages);
        vendor.helpers.set_classname_and_id(number_of_messages, "chatblock__avatar_number-of-messages");
        gtk_widget_set_size_request(number_of_messages, 30, 16);
        gtk_fixed_put(GTK_FIXED(fixed_container), number_of_messages, 25, 32);
    }

    return fixed_container;
}
