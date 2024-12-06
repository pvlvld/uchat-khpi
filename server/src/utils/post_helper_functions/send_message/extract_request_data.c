#include "utils.h"
#include <../../../libraries/cJSON/cJSON.h>
#include <string.h>

char *strdup(const char *src) {
    char *dst = malloc(strlen(src) + 1);
    if (dst == NULL) return NULL;
    strcpy(dst, src);
    return dst;
}


char *extract_recipient_login(cJSON *json) {
    if (!json) { return NULL; }

    cJSON *recipient_login_item = cJSON_GetObjectItem(json, "recipient_login");
    if (!cJSON_IsString(recipient_login_item)) { return NULL; }

    // Duplicate the recipient login string to return (remember to free it later)
    return strdup(recipient_login_item->valuestring);
}

char *extract_recipient_username(cJSON *json) {
    if (!json) { return NULL; }

    cJSON *recipient_username_item = cJSON_GetObjectItem(json, "username");
    if (!cJSON_IsString(recipient_username_item)) { return NULL; }

    // Duplicate the recipient login string to return (remember to free it later)
    return strdup(recipient_username_item->valuestring);
}

char *extract_message(cJSON *json) {
    if (!json) { return NULL; }

    // Get the message field
    cJSON *message_item = cJSON_GetObjectItem(json, "message");
    if (!cJSON_IsString(message_item)) { return NULL; }

    // Duplicate the message string to return (remember to free it later)
    return strdup(message_item->valuestring);
}

char *extract_chat_id(cJSON *json) {
    if (!json) { return NULL; }

    // Get the chat_id field
    cJSON *chat_id_item = cJSON_GetObjectItem(json, "chat_id");
    if (!cJSON_IsString(chat_id_item)) { return NULL; }

    // Duplicate the chat_id string to return (remember to free it later)
    return strdup(chat_id_item->valuestring);
}

char *extract_message_id(cJSON *json) {
    if (!json) {
        return NULL;
    }

    // Get the message_id field
    cJSON *message_id_item = cJSON_GetObjectItem(json, "message_id");
    if (!cJSON_IsString(message_id_item)) {
        return NULL;
    }

    // Duplicate the message_id string to return (remember to free it later)
    return strdup(message_id_item->valuestring);
}

char *extract_user_id(cJSON *json) {
    if (!json) { return NULL; }

    cJSON *user_id_item = cJSON_GetObjectItem(json, "user_id");
    if (!cJSON_IsString(user_id_item)) { return NULL; }

    // Duplicate the recipient login string to return (remember to free it later)
    return strdup(user_id_item->valuestring);
}

