#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../../libraries/cJSON/cJSON.h"

#define BUFFER_SIZE 4096

typedef struct {
    void (*handle_handshake)(SSL *ssl, const char *key);
    void (*process_websocket_frame)(SSL *ssl, const unsigned char *frame, ssize_t frame_size);
    void (*send_close_frame)(SSL *ssl, uint16_t status_code);
    void (*send_websocket_message)(SSL *ssl, const char *message);
    char *(*extract_from_ws)(const char *key, const char *request);
} t_websocket;

void _handle_handshake(SSL *ssl, const char *key);
void _process_websocket_frame(SSL *ssl, const unsigned char *frame, ssize_t frame_size);
void _send_close_frame(SSL *ssl, uint16_t status_code) ;
void _send_websocket_message(SSL *ssl, const char *message);
char *_extract_from_ws(const char *key, const char *request);
t_websocket init_websocket(void);

#endif // WEBSOCKET_H
