#include "../../inc/header.h"
#include <sys/time.h>

static time_t parse_timestamp(const char *timestamp_str) {
    struct tm tm = {0};

    if (sscanf(timestamp_str, "%4d-%2d-%2d %2d:%2d:%2d",
               &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
               &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6) {
        fprintf(stderr, "Invalid timestamp format\n");
        return -1;
               }

    tm.tm_year -= 1900;
    tm.tm_mon -= 1;

    time_t timestamp = timegm(&tm);

    if (timestamp == -1) {
        fprintf(stderr, "Failed to convert timestamp to time_t\n");
        return -1;
    }

    return timestamp;
}

static gboolean delayed_message_receipt(gpointer user_data) {
    t_api_message_struct *api_message = (t_api_message_struct *)user_data;

    message_receipt(vendor.pages.main_page.sidebar.widget, api_message, 0);

    return G_SOURCE_REMOVE;
}

static void update_chats(cJSON *personal_chats_json) {
    cJSON *personal_chat = NULL;
    cJSON_ArrayForEach(personal_chat, personal_chats_json) {
        int chat_id = cJSON_GetObjectItem(personal_chat, "chat_id")->valueint;
        cJSON *new_messages_json = cJSON_GetObjectItem(personal_chat, "new_messages");
        cJSON *new_message = NULL;
        cJSON_ArrayForEach(new_message, new_messages_json) {
            int deleted = cJSON_GetObjectItem(new_message, "deleted")->valueint;

            if (!deleted) {
                int message_id = cJSON_GetObjectItem(new_message, "message_id")->valueint;
                char *content = cJSON_GetObjectItem(new_message, "content")->valuestring;

                cJSON *sender_id_item = cJSON_GetObjectItem(new_message, "sender_id");
                int sender_id = (sender_id_item != NULL) ? sender_id_item->valueint : vendor.current_user.user_id;

                cJSON *timestamp_item = cJSON_GetObjectItem(new_message, "timestamp");

                t_api_message_struct *api_message = malloc(sizeof(t_api_message_struct));
                api_message->chat_id = chat_id;
                api_message->sender_id = sender_id;
                api_message->message_id = message_id;
                if (timestamp_item->valuestring[0] != '\0') {
                    api_message->timestamp = parse_timestamp(timestamp_item->valuestring);
                } else {
                    api_message->timestamp = time(NULL);
                }

                char *message = content;

                char *decrypted_message = message;

                t_chats_struct *chat_struct = vendor.database.tables.chats_table.get_chat_by_id(api_message->chat_id);

                if (chat_struct != NULL) {
                    if (chat_struct->chat_type == PERSONAL) {
                        decrypted_message = vendor.helpers.strdup(vendor.crypto.decrypt_data_from_db(message));
                    }

                    api_message->message_encrypted = vendor.crypto.encrypt_data_for_db(vendor.crypto.public_key_str, decrypted_message);

                    g_timeout_add(100, delayed_message_receipt, api_message);
                }
            }
        }
    }
}

gboolean get_all_updates(gpointer user_data) {
    (void)user_data;

    cJSON *json_body = cJSON_CreateObject();
    char oldest_timestamp[32];
    snprintf(oldest_timestamp, sizeof(oldest_timestamp), "%ld", vendor.database.sql.get_oldest_timestamp() + 1);

    char user_id[32];
    snprintf(user_id, sizeof(user_id), "%d", vendor.current_user.user_id);

    cJSON_AddStringToObject(json_body, "timestamp", vendor.helpers.strdup(oldest_timestamp));
    cJSON_AddStringToObject(json_body, "user_id", vendor.helpers.strdup(user_id));
    cJSON_AddStringToObject(json_body, "token", vendor.helpers.strdup(vendor.current_user.jwt));

    cJSON *response = vendor.ssl_struct.send_request("GET", "/get_all_updates", json_body);
    cJSON *code = cJSON_GetObjectItem(response, "code");
    if (strcmp(code->valuestring, "SUCCESS") == 0) {
        cJSON *updates = cJSON_GetObjectItem(response, "updates");

        if (updates != NULL) {
            cJSON *personal_chats_json = cJSON_GetObjectItem(updates, "personal_chats");
            update_chats(personal_chats_json);

            cJSON *group_chats_json = cJSON_GetObjectItem(updates, "group_chats");
            update_chats(group_chats_json);
        }
    } else {
        g_print("Error with updates\n");
    }

    return FALSE;
}
