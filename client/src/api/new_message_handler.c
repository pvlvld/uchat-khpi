#include "../../inc/header.h"

static time_t parse_timestamp(const char *timestamp_str) {
    struct tm tm;

    if (sscanf(timestamp_str, "%4d-%2d-%2d %2d:%2d:%2d",
               &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
               &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6) {
        fprintf(stderr, "Invalid timestamp format\n");
        return -1;
               }

    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    tm.tm_hour += 2;

    time_t timestamp = mktime(&tm);

    if (timestamp == -1) {
        fprintf(stderr, "Failed to convert timestamp to time_t\n");
        return -1;
    }

    return timestamp;
}

gboolean new_message_handler(gpointer user_data) {
    cJSON *req = (cJSON *)user_data;
    t_api_message_struct *api_message = malloc(sizeof(t_api_message_struct));
    api_message->chat_id = cJSON_GetObjectItem(req, "chat_id")->valueint;
    api_message->sender_id = cJSON_GetObjectItem(req, "sender_id")->valueint;
    api_message->message_id = cJSON_GetObjectItem(req, "message_id")->valueint;
    api_message->timestamp = parse_timestamp(cJSON_GetObjectItem(req, "timestamp")->valuestring);
    char *message = cJSON_GetObjectItem(req, "message_text")->valuestring;
    char *decrypted_message = vendor.crypto.decrypt_data_from_db(message);

    api_message->message_encrypted = vendor.crypto.encrypt_data_for_db(vendor.crypto.public_key_str, decrypted_message);

    message_receipt(vendor.pages.main_page.sidebar.widget, api_message);

    return FALSE;
}
