#ifndef SSL_H
#define SSL_H

#include "../../libraries/openssl/ssl.h"
#include "../../libraries/openssl/err.h"

SSL_CTX *init_ssl_context(void);

#endif // SSL_H
