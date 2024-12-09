#include "../../inc/header.h"

gboolean added_to_group_handler(gpointer user_data) {
    cJSON *req = (cJSON *)user_data;

    int chat_id = cJSON_GetObjectItem(req, "chat_id")->valueint;
    int sender_id = cJSON_GetObjectItem(req, "sender_id")->valueint;
    char *group_name = cJSON_GetObjectItem(req, "group_name")->valuestring;
    vendor.database.tables.group_chats_table.create_group(chat_id, group_name);
    vendor.database.tables.group_chat_members_table.add_member(chat_id, sender_id, "member");
    vendor.database.tables.group_chat_members_table.add_member(chat_id, vendor.current_user.user_id, "member");

    t_chat_info *chat_info = parse_chat_info_by_id(chat_id);
    GtkWidget *chatblock = vendor.pages.main_page.sidebar.create_chatblock(chat_info);
    if (!chatblock) {
        printf("[ERROR] Failed to create a block for chat with ID: %d\n", chat_info->id);
        return FALSE;
    }

    g_object_set_data(G_OBJECT(chatblock), "chat_info", chat_info);
    gtk_box_pack_start(GTK_BOX(vendor.sidebar.stretchable_box), chatblock, FALSE, FALSE, 0);
    gtk_box_reorder_child(GTK_BOX(vendor.sidebar.stretchable_box), chatblock, 0);
    gtk_widget_show_all(vendor.sidebar.stretchable_box);

    return FALSE;
}
