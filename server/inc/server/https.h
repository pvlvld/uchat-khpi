#ifndef HTTPS_H
#define HTTPS_H

#include <openssl/ssl.h>

typedef struct {
    void (*handle_https_request)(SSL *ssl, const char *request);
    void (*send_https_response)(SSL *ssl, const char *status, const char *content_type, const char *body);
} t_https;

void _handle_https_request(SSL *ssl, const char *request);
void _send_https_response(SSL *ssl, const char *status, const char *content_type, const char *body);
t_https init_https(void);

#endif //HTTPS_H
