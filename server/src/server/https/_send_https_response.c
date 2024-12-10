#include "../../../inc/server/https.h"
#include "../../../inc/header.h"

void _send_https_response(SSL *ssl, const char *status, const char *content_type, const char *body) {
    size_t body_length = strlen(body);
    size_t response_size = strlen(status) + strlen(content_type) + body_length + 100; // Оценка необходимого размера (учитываем заголовки и '\r\n')

    char *response = (char *)malloc(response_size);
    if (response == NULL) {
        perror("Failed to allocate memory for response");
        return;
    }

    int len = snprintf(response, response_size,
                       "HTTP/1.1 %s\r\n"
                       "Content-Type: %s\r\n"
                       "Content-Length: %lu\r\n"
                       "\r\n"
                       "%s",
                       status, content_type, body_length, body);

    if (len < 0 || (size_t) len >= response_size) {
        fprintf(stderr, "Error formatting HTTP response\n");
        free(response);
        return;
    }

    if (SSL_write(ssl, response, len) <= 0) {
        ERR_print_errors_fp(stderr);
    }

    free(response);
}
