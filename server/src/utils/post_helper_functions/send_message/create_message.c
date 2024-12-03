#include "../../../../../libraries/cJSON/cJSON.h"
#include "utils.h"

cJSON *create_message_json(int sender_id, const char *message_text) {
    cJSON *message_json = cJSON_CreateObject();
    if (!message_json) return NULL;

    cJSON_AddNumberToObject(message_json, "sender_id", sender_id);
    cJSON_AddStringToObject(message_json, "message", message_text);

    return message_json;
}

cJSON *create_delete_message_json(int sender_id, deleteMessageResult_t delete_result) {
    // Create the root JSON object
    cJSON *message_json = cJSON_CreateObject();
    if (!message_json) return NULL;

    // Add the main "message" field
    cJSON_AddStringToObject(message_json, "message", "Message deleted");

    // Create the "details" object
    cJSON *details = cJSON_CreateObject();
    if (!details) {
        cJSON_Delete(message_json);
        return NULL;
    }

    cJSON_AddNumberToObject(details, "sender_id", sender_id);
    cJSON_AddStringToObject(details, "operation", "delete");
    cJSON_AddNumberToObject(details, "message_id", delete_result.message_id);
    cJSON_AddStringToObject(details, "timestamp", delete_result.timestamp);

    // Add the "details" object to the root JSON object
    cJSON_AddItemToObject(message_json, "details", details);

    return message_json;
}





