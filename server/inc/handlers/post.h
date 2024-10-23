#ifndef POST_H
#define POST_H

#include "handlers.h"

void post_rout(SSL *ssl, const char *request);
void login_rout(SSL *ssl, const char *request);

typedef struct {
    void (*post)(SSL *ssl, const char *request); // No underscores because this will be middleware.
    void (*_login)(SSL *ssl, const char *request);
    t_middleware middleware;
} t_post_handler;

#endif //POST_H
