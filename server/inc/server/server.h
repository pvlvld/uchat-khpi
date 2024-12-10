#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>
#include <arpa/inet.h>
#include "../../../libraries/openssl/err.h"
#include "client.h"
#include "https.h"

void start_server(void);

typedef struct {
	unsigned short port;
    SSL_CTX *ssl_ctx;
    t_client_settings client_settings;
    t_https https;
} t_server;

t_server init_server(void);

#endif //SERVER_H
