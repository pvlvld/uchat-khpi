#include "../../inc/header.h"

void friend_request_handler(cJSON *req) {
    t_users_struct *friend_struct = malloc(sizeof(t_users_struct));
    friend_struct->user_id = cJSON_GetObjectItem(req, "sender_id")->valueint;
    friend_struct->username = cJSON_GetObjectItem(req, "sender_username")->valuestring;
    friend_struct->user_login = cJSON_GetObjectItem(req, "sender_login")->valuestring;
    friend_struct->about = cJSON_GetObjectItem(req, "sender_about")->valuestring;
    friend_struct->public_key = cJSON_GetObjectItem(req, "sender_public_key")->valuestring;

    int chat_id = cJSON_GetObjectItem(req, "chat_id")->valueint;
    const char *timestamp_str = cJSON_GetObjectItem(req, "timestamp")->valuestring;

    if (timestamp_str != NULL) {
        struct tm tm;
        if (sscanf(timestamp_str, "%4d-%2d-%2d %2d:%2d:%2d",
            &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
            &tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6) {
            tm.tm_year -= 1900;
            tm.tm_mon -= 1;
            time_t timestamp = mktime(&tm);
            friend_struct->updated_at = *localtime(&timestamp);
            } else {
                printf("[ERROR] Invalid timestamp format\n");
            }
    } else {
        printf("[ERROR] Timestamp is NULL\n");
    }

    vendor.database.tables.users_table.add_user(friend_struct);

    vendor.database.tables.chats_table.add_chat(chat_id, "personal");
    vendor.database.tables.personal_chats_table.create_personal_chat(chat_id, friend_struct->user_id);

    t_chat_info *chat_info = parse_chat_info_by_id(chat_id);
    GtkWidget *chatblock = vendor.pages.main_page.sidebar.create_chatblock(chat_info);
    if (!chatblock) {
        printf("[ERROR] Failed to create a block for chat with ID: %d\n", chat_info->id);
        return;
    }

    g_object_set_data(G_OBJECT(chatblock), "chat_info", chat_info);
    gtk_box_pack_start(GTK_BOX(vendor.sidebar.stretchable_box), chatblock, FALSE, FALSE, 0);
    gtk_box_reorder_child(GTK_BOX(vendor.sidebar.stretchable_box), chatblock, 0);
    gtk_widget_show_all(vendor.sidebar.stretchable_box);
}
