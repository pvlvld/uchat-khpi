#include "../../inc/websocket.h"
#include "../../inc/header.h"

void _process_websocket_frame(SSL *ssl, const unsigned char *frame, ssize_t frame_size) {
    if (frame_size < 2) {
        return;
    }

    if ((frame[0] & 0x0F) == 0x08) {
        if (vendor.env.dev_mode) printf("Received WebSocket close frame\n");
        _send_close_frame(ssl, 1000); // Используем стандартный код 1000 для нормального закрытия
        return;
    }

    size_t payload_length = frame[1] & 0x7F;
    size_t mask_offset = 2;
    if (payload_length == 126) {
        mask_offset = 4;
    } else if (payload_length == 127) {
        mask_offset = 10;
    }

    if (frame_size < (ssize_t)(mask_offset + 4 + payload_length)) {
        return;
    }

    unsigned char mask[4];
    memcpy(mask, &frame[mask_offset], 4);

    unsigned char payload[BUFFER_SIZE];
    for (size_t i = 0; i < payload_length; i++) {
        payload[i] = frame[mask_offset + 4 + i] ^ mask[i % 4];
    }

    payload[payload_length] = '\0';

   if (vendor.env.dev_mode) printf("Received WebSocket message: %s\n", payload);
}
