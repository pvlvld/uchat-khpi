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

    message_receipt(vendor.pages.main_page.sidebar.widget, api_message, 1);

    return G_SOURCE_REMOVE;
}

gboolean new_message_handler(gpointer user_data) {
    cJSON *req = (cJSON *)user_data;
    t_api_message_struct *api_message = malloc(sizeof(t_api_message_struct));
    api_message->chat_id = cJSON_GetObjectItem(req, "chat_id")->valueint;
    api_message->sender_id = cJSON_GetObjectItem(req, "sender_id")->valueint;

    if (api_message->sender_id == vendor.current_user.user_id) {
        return FALSE;
    }

    api_message->message_id = cJSON_GetObjectItem(req, "message_id")->valueint;
    api_message->timestamp = parse_timestamp(cJSON_GetObjectItem(req, "timestamp")->valuestring);
    char *message = cJSON_GetObjectItem(req, "message_text")->valuestring;

    char *decrypted_message = message;


    if (vendor.database.tables.chats_table.get_chat_by_id(api_message->chat_id)->chat_type == PERSONAL) {
        decrypted_message = vendor.helpers.strdup(vendor.crypto.decrypt_data_from_db(message));
    }

    api_message->message_encrypted = vendor.crypto.encrypt_data_for_db(vendor.crypto.public_key_str, decrypted_message);

//    message_receipt(vendor.pages.main_page.sidebar.widget, api_message);
    g_timeout_add(100, delayed_message_receipt, api_message);

    return FALSE;
}
