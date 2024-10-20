#include "../../inc/websocket.h"

void _send_websocket_message(SSL *ssl, const char *message) {
    unsigned char frame[BUFFER_SIZE];
    size_t message_length = strlen(message);
    size_t header_size = 2; // Base header size

    frame[0] = 0x81; // FIN, text frame

    if (message_length <= 125) {
        frame[1] = (unsigned char)message_length;
        header_size = 2;
    } else if (message_length <= 65535) {
        frame[1] = 126;
        frame[2] = (message_length >> 8) & 0xFF;
        frame[3] = message_length & 0xFF;
        header_size = 4;
    } else {
        frame[1] = 127;
        frame[2] = (message_length >> 56) & 0xFF;
        frame[3] = (message_length >> 48) & 0xFF;
        frame[4] = (message_length >> 40) & 0xFF;
        frame[5] = (message_length >> 32) & 0xFF;
        frame[6] = (message_length >> 24) & 0xFF;
        frame[7] = (message_length >> 16) & 0xFF;
        frame[8] = (message_length >> 8) & 0xFF;
        frame[9] = message_length & 0xFF;
        header_size = 10;
    }

    // Check if the total size fits within the buffer
    if (header_size + message_length > BUFFER_SIZE) {
        fprintf(stderr, "Message too long to fit in buffer\n");
        return;
    }

    // Copy the message to the frame
    memcpy(&frame[header_size], message, message_length);

    // Send the frame
    SSL_write(ssl, frame, header_size + message_length);
}
