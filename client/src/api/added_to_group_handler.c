#include "../../inc/header.h"

static t_users_struct *add_friend(char *username) {
    cJSON *json_body = cJSON_CreateObject();
    cJSON_AddStringToObject(json_body, "username", vendor.helpers.strdup(username));

    cJSON *response = vendor.ssl_struct.send_request("POST", "/friend_request", json_body);
    cJSON *error = cJSON_GetObjectItem(response, "error");
    if (error->valueint == 0) {
        t_users_struct *friend_struct = malloc(sizeof(t_users_struct));
        friend_struct->user_id = cJSON_GetObjectItem(response, "recipient_id")->valueint;
        friend_struct->username = cJSON_GetObjectItem(response, "recipient_login")->valuestring;
        friend_struct->user_login = cJSON_GetObjectItem(response, "recipient_login")->valuestring;
        friend_struct->about = cJSON_GetObjectItem(response, "recipient_about")->valuestring;
        friend_struct->public_key = cJSON_GetObjectItem(response, "recipient_public_key")->valuestring;

        int chat_id = cJSON_GetObjectItem(response, "chat_id")->valueint;
        const char *timestamp_str = cJSON_GetObjectItem(response, "timestamp")->valuestring;

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
            return NULL;
        }

        g_object_set_data(G_OBJECT(chatblock), "chat_info", chat_info);
        gtk_box_pack_start(GTK_BOX(vendor.sidebar.stretchable_box), chatblock, FALSE, FALSE, 0);
        gtk_box_reorder_child(GTK_BOX(vendor.sidebar.stretchable_box), chatblock, 0);
        gtk_widget_show_all(vendor.sidebar.stretchable_box);
        vendor.modal.add_friend.destroy();

        return friend_struct;
    }

    return NULL;
}

gboolean added_to_group_handler(gpointer user_data) {
    cJSON *req = (cJSON *)user_data;
    g_print("added_to_group_handler\n");

    int chat_id = cJSON_GetObjectItem(req, "chat_id")->valueint;
//    int sender_id = cJSON_GetObjectItem(req, "sender_id")->valueint;
    char *group_name = cJSON_GetObjectItem(req, "group_name")->valuestring;
    vendor.database.tables.group_chats_table.create_group(chat_id, group_name);
    g_print("create_group\n");

    cJSON *group_members_json = cJSON_GetObjectItem(req, "group_members");
    if (!cJSON_IsArray(group_members_json)) {
        printf("[ERROR] group_members is not an array\n");
        return FALSE;
    }

    cJSON *member_json = NULL;
    cJSON_ArrayForEach(member_json, group_members_json) {
        char *member_username = cJSON_GetObjectItem(member_json, "member_username")->valuestring;
        int member_id = cJSON_GetObjectItem(member_json, "member_id")->valueint;

        t_users_struct *user = vendor.database.tables.users_table.get_user_by_username(member_username);
        if (user == NULL) {
            user = add_friend(member_username);
        }
        if (user == NULL) {
            continue;
        } else {
            g_print("Add member\n");
            vendor.database.tables.group_chat_members_table.add_member(chat_id, user->user_id, member_id == 1 ? "owner" : "member");
        }
    }

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
