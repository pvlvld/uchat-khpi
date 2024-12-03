#include "../../../inc/header.h"
#include "../../../inc/server/client.h"

void _send_message_to_client(int user_id, cJSON *message) { // sender_id and message are mandatory fields in message object
    // Validate that `message` is a valid JSON object
    if (!cJSON_IsObject(message)) {
        fprintf(stderr, "Message is not a valid JSON object\n");
        return;
    }

    // Validate mandatory fields: `sender_id` and `message`
    cJSON *sender_id_item = cJSON_GetObjectItem(message, "sender_id");
    cJSON *message_item = cJSON_GetObjectItem(message, "message");

    if (!cJSON_IsNumber(sender_id_item) || !cJSON_IsString(message_item)) {
        fprintf(stderr, "Missing or invalid mandatory fields in JSON message: sender_id or message\n");
        return;
    }

    // Convert the entire message object to a JSON string
    char *json_str = cJSON_PrintUnformatted(message);
    if (!json_str) {
        fprintf(stderr, "Failed to convert cJSON to string\n");
        return;
    }

    size_t json_len = strlen(json_str);
    size_t frame_size;
    unsigned char *frame;

    // Calculate frame size
    if (json_len <= 125) {
        frame_size = 2 + json_len;
    } else if (json_len <= 65535) {
        frame_size = 4 + json_len;
    } else {
        frame_size = 10 + json_len;
    }

    frame = malloc(frame_size);
    if (!frame) {
        perror("malloc");
        free(json_str);
        return;
    }

    // Set WebSocket frame bytes
    frame[0] = 0x81; // Final text frame
    if (json_len <= 125) {
        frame[1] = (unsigned char)json_len;
        memcpy(&frame[2], json_str, json_len);
    } else if (json_len <= 65535) {
        frame[1] = 126;
        frame[2] = (unsigned char)((json_len >> 8) & 0xFF);
        frame[3] = (unsigned char)(json_len & 0xFF);
        memcpy(&frame[4], json_str, json_len);
    } else {
        frame[1] = 127;
        frame[2] = (unsigned char)((json_len >> 56) & 0xFF);
        frame[3] = (unsigned char)((json_len >> 48) & 0xFF);
        frame[4] = (unsigned char)((json_len >> 40) & 0xFF);
        frame[5] = (unsigned char)((json_len >> 32) & 0xFF);
        frame[6] = (unsigned char)((json_len >> 24) & 0xFF);
        frame[7] = (unsigned char)((json_len >> 16) & 0xFF);
        frame[8] = (unsigned char)((json_len >> 8) & 0xFF);
        frame[9] = (unsigned char)(json_len & 0xFF);
        memcpy(&frame[10], json_str, json_len);
    }

    pthread_mutex_lock(&vendor.server.client_settings.clients_mutex);
    for (unsigned int i = 0; i < vendor.server.client_settings.max_clients; i++) {
        if (vendor.server.client_settings.clients[i].user_id == user_id) {
            if (SSL_write(vendor.server.client_settings.clients[i].ssl, frame, frame_size) <= 0) {
                ERR_print_errors_fp(stderr);
            }
            break;
        }
    }
    pthread_mutex_unlock(&vendor.server.client_settings.clients_mutex);

    free(frame);
    free(json_str);
}
