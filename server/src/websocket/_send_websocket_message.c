#include "../../inc/websocket.h"

void _send_websocket_message(SSL *ssl, const char *message) {
    size_t message_length = strlen(message);
    size_t header_size = 2; // Base header size

    // Calculate the total size needed for the frame
    size_t total_size = header_size + message_length;

    // If the message is larger than 125 bytes, handle the larger frames
    if (message_length <= 125) {
        header_size = 2;
    } else if (message_length <= 65535) {
        header_size = 4;
        total_size = header_size + message_length;
    } else {
        header_size = 10;
        total_size = header_size + message_length;
    }

    // Dynamically allocate memory for the frame
    unsigned char *frame = malloc(total_size);
    if (!frame) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    frame[0] = 0x81; // FIN, text frame

    // Set the appropriate header values
    if (message_length <= 125) {
        frame[1] = (unsigned char)message_length;
    } else if (message_length <= 65535) {
        frame[1] = 126;
        frame[2] = (message_length >> 8) & 0xFF;
        frame[3] = message_length & 0xFF;
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
    }

    // Copy the message to the frame
    memcpy(&frame[header_size], message, message_length);

    // Send the frame
    SSL_write(ssl, frame, total_size);

    // Free the dynamically allocated memory
    free(frame);
}



