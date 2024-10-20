#include "../../../inc/server/client.h"
#include "../../../inc/header.h"

void _add_client(t_client client) {
    pthread_mutex_lock(&vendor.server.client_settings.clients_mutex);
    for (unsigned int i = 0; i < vendor.server.client_settings.max_clients; i++) {
        if (vendor.server.client_settings.clients[i].ssl == NULL) {
            vendor.server.client_settings.clients[i] = client;
            vendor.server.client_settings.client_count++;
           if (vendor.env.dev_mode) printf("Added client with user_id: %d at index %d\n", client.user_id, i);
            break;
        }
    }
    pthread_mutex_unlock(&vendor.server.client_settings.clients_mutex);
}

void _update_client_user_id(SSL *ssl, int user_id) {
    pthread_mutex_lock(&vendor.server.client_settings.clients_mutex);
    for (unsigned int i = 0; i < vendor.server.client_settings.max_clients; i++) {
        if (vendor.server.client_settings.clients[i].ssl == ssl) {
            vendor.server.client_settings.clients[i].user_id = user_id;
           if (vendor.env.dev_mode) printf("Updated client with user_id: %d at index %d\n", user_id, i);
            break;
        }
    }
    pthread_mutex_unlock(&vendor.server.client_settings.clients_mutex);
}

void _remove_client(SSL *ssl) {
    pthread_mutex_lock(&vendor.server.client_settings.clients_mutex);
    for (unsigned int i = 0; i < vendor.server.client_settings.max_clients; i++) {
        if (vendor.server.client_settings.clients[i].ssl == ssl && vendor.server.client_settings.clients[i].user_id != -1) {
           if (vendor.env.dev_mode) printf("Removing client with user_id: %d from index %d\n", vendor.server.client_settings.clients[i].user_id, i);
            vendor.server.client_settings.clients[i].ssl = NULL;
            vendor.server.client_settings.clients[i].socket = -1;
            vendor.server.client_settings.clients[i].user_id = -1;
            vendor.server.client_settings.client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&vendor.server.client_settings.clients_mutex);
}
