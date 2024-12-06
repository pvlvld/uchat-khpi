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

typedef struct {
  SSL *ssl;
  int sock;
  int running;
  pthread_mutex_t lock;
} t_ws_client;

t_ssl_struct init_ssl_struct(void);

void disconnect_websocket(void);
void *websocket_thread(void *arg);
void *connect_websocket(void *arg);

#endif //SERVER_CONNECT_H
