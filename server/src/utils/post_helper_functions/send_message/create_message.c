#include "../../../../../libraries/cJSON/cJSON.h"

cJSON *create_message_json(int sender_id, const char *message_text) {
    cJSON *message_json = cJSON_CreateObject();
    if (!message_json) return NULL;

    cJSON_AddNumberToObject(message_json, "sender_id", sender_id);
    cJSON_AddStringToObject(message_json, "message", message_text);

    return message_json;
}
