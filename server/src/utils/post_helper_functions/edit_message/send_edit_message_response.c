#include "../../../../inc/handlers/post_handlers/edit_message.h"

void send_edit_message_response(SSL *ssl, EditMessageResult_t *edit_message_result, cJSON *response_json) {

    if (edit_message_result->Error == 1) {
        cJSON_AddBoolToObject(response_json, "error", true);

        // Map error codes to response details
        if (strcmp(edit_message_result->error_code, "UNKNOWN_CHAT_TYPE") == 0) {
            cJSON_AddStringToObject(response_json, "code", "UNKNOWN_CHAT_TYPE");
            cJSON_AddStringToObject(response_json, "message", "The chat type is unknown or unsupported.");
            vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        } else if (strcmp(edit_message_result->error_code, "USER_ROLE_NOT_FOUND") == 0) {
            cJSON_AddStringToObject(response_json, "code", "USER_ROLE_NOT_FOUND");
            cJSON_AddStringToObject(response_json, "message", "The user's role in the group or channel could not be determined.");
            vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json", cJSON_Print(response_json));
        } else if (strcmp(edit_message_result->error_code, "MESSAGE_SENDER_NOT_FOUND") == 0) {
            cJSON_AddStringToObject(response_json, "code", "MESSAGE_SENDER_NOT_FOUND");
            cJSON_AddStringToObject(response_json, "message", "The sender of the message could not be found.");
            vendor.server.https.send_https_response(ssl, "404 Not Found", "application/json", cJSON_Print(response_json));
        } else if (strcmp(edit_message_result->error_code, "USER_NOT_ALLOWED_TO_EDIT_THIS_MESSAGE") == 0) {
            cJSON_AddStringToObject(response_json, "code", "FORBIDDEN");
            cJSON_AddStringToObject(response_json, "message", "The user is not allowed to edit this message.");
            vendor.server.https.send_https_response(ssl, "403 Forbidden", "application/json", cJSON_Print(response_json));
        } else if (strcmp(edit_message_result->error_code, "EDIT_MESSAGE_DB_ERROR") == 0) {
            cJSON_AddStringToObject(response_json, "code", "DB_ERROR");
            cJSON_AddStringToObject(response_json, "message", "An error occurred while editing the message in the database.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        } else if (strcmp(edit_message_result->error_code, "TIMESTAMP_QUERY_ERROR") == 0) {
            cJSON_AddStringToObject(response_json, "code", "TIMESTAMP_ERROR");
            cJSON_AddStringToObject(response_json, "message", "An error occurred while retrieving the timestamp.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        } else {
            cJSON_AddStringToObject(response_json, "code", "UNKNOWN_ERROR");
            cJSON_AddStringToObject(response_json, "message", "An unknown error occurred.");
            vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        }
    } else {
        cJSON_AddBoolToObject(response_json, "error", false);
        cJSON_AddStringToObject(response_json, "code", "SUCCESS");
        cJSON_AddStringToObject(response_json, "message", "Message edited successfully.");
        cJSON_AddNumberToObject(response_json, "message_id", edit_message_result->message_id);
        cJSON_AddStringToObject(response_json, "timestamp", edit_message_result->timestamp);
        vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));
    }

    // Cleanup
    cJSON_Delete(response_json);
}
