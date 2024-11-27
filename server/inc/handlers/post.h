#ifndef POST_H
#define POST_H

#include "handlers.h"

void post_rout(SSL *ssl, const char *request);
void login_rout(SSL *ssl, const char *request);
void register_rout(SSL *ssl, const char *request);
void protected_send_message_rout(SSL *ssl, const char *request);
void protected_delete_message_rout(SSL *ssl, const char *request);
void protected_edit_message_rout(SSL *ssl, const char *request);

typedef struct {
    void (*post)(SSL *ssl, const char *request); // No underscores because this will be middleware.
    void (*_login)(SSL *ssl, const char *request);
    void (*_register)(SSL *ssl, const char *request);
    void (*_send_message)(SSL *ssl, const char *request);
    void (*_delete_message)(SSL *ssl, const char *request);
    void (*_edit_message)(SSL *ssl, const char *request);
    t_middleware middleware;
} t_post_handler;

#endif //POST_H
