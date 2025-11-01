#ifndef SSL_H
#define SSL_H

#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX *init_ssl_context(void);

#endif // SSL_H
