#ifndef CLIENT_H
#define CLIENT_H

#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/err.h>
#include "../../../libraries/cJSON/cJSON.h"

typedef struct {
    int socket;
    SSL *ssl;
    int user_id;
} t_client;

typedef struct {
	pthread_mutex_t clients_mutex;
	unsigned int max_clients;
	unsigned int client_count;
	t_client *clients;
    void *(*handle_client)(void *client_socket);
    void (*add_client)(t_client client);
    void (*update_client_user_id)(SSL *ssl, int user_id);
    void (*remove_client)(SSL *ssl);
    void (*send_message_to_client)(int user_id, cJSON *message_json);
} t_client_settings;

void *_handle_client(void *client_socket);
void _add_client(t_client client);
void _update_client_user_id(SSL *ssl, int user_id);
void _remove_client(SSL *ssl);
void _send_message_to_client(int user_id, cJSON *message_json);

t_client_settings init_client_settings(void);

#endif //CLIENT_H
