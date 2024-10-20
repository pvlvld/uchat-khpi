#include "../../inc/websocket.h"

void _send_close_frame(SSL *ssl, uint16_t status_code) {
    unsigned char close_frame[4] = {0x88, 0x02, (status_code >> 8) & 0xFF, status_code & 0xFF};
    if (SSL_write(ssl, close_frame, sizeof(close_frame)) <= 0) {
        ERR_print_errors_fp(stderr);
    }
}
