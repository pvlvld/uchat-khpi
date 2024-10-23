#ifndef GET_H
#define GET_H

#include "handlers.h"

void hello_rout(SSL *ssl, const char *request);
void secret_rout(SSL *ssl, const char *request);

typedef struct {
    void (*secret)(SSL *ssl, const char *request); // No underscores because this will be middleware.
    void (*_hello)(SSL *ssl, const char *request);
    t_middleware middleware;
} t_get_handler;

#endif //GET_H
