#include "../../../inc/server/https.h"
#include "../../../inc/header.h"

void _send_https_response(SSL *ssl, const char *status, const char *content_type, const char *body) {
    char response[BUFFER_SIZE];
    int len = snprintf(response, sizeof(response),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %lu\r\n"
             "\r\n"
             "%s",
             status, content_type, strlen(body), body);

    if (SSL_write(ssl, response, len) <= 0) {
        ERR_print_errors_fp(stderr);
    }
}
