#include "../../inc/websocket.h"

void _handle_handshake(SSL *ssl, const char *sec_websocket_key) {
    char response[BUFFER_SIZE];
    unsigned char sha1_hash[SHA_DIGEST_LENGTH];
    char base64_sha1_hash[29]; // 28 + 1 for null terminator

    // Concatenate the Sec-WebSocket-Key with the GUID
    char concatenated[BUFFER_SIZE];
    snprintf(concatenated, sizeof(concatenated), "%s%s", sec_websocket_key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

    // Compute SHA-1 hash
    SHA1((unsigned char *)concatenated, strlen(concatenated), sha1_hash);

    // Base64 encode the SHA-1 hash
    EVP_EncodeBlock((unsigned char *)base64_sha1_hash, sha1_hash, SHA_DIGEST_LENGTH);

    // Create the response
    snprintf(response, sizeof(response),
             "HTTP/1.1 101 Switching Protocols\r\n"
             "Upgrade: websocket\r\n"
             "Connection: Upgrade\r\n"
             "Sec-WebSocket-Accept: %s\r\n\r\n",
             base64_sha1_hash);

    // Send the response
    SSL_write(ssl, response, strlen(response));
}
