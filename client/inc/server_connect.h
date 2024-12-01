#ifndef SERVER_CONNECT_H
#define SERVER_CONNECT_H

#include "../../libraries/openssl/ssl.h"
#include "../../libraries/openssl/err.h"

#define BUFFER_SIZE 4096

typedef struct {
  void (*init_openssl)(void);
  void (*cleanup_openssl)(void);
  int sock;
  SSL *ssl;
  cJSON *(*send_request)(const char *method, const char *path, cJSON *json_body);
} t_ssl_struct;

t_ssl_struct init_ssl_struct(void);

#endif //SERVER_CONNECT_H
