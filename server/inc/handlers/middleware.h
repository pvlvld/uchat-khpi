#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include "../header.h"

void jwt_middleware(SSL *ssl, const char *request, void (*next)(SSL *ssl, const char *request));

typedef struct {
    void (*_jwt)(SSL *ssl, const char *request, void (*next)(SSL *ssl, const char *request)); // Функция middleware для JWT
} t_middleware;

#endif //MIDDLEWARE_H
