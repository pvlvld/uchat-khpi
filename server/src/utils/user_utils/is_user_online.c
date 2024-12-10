#include <pthread.h>
#include <stdbool.h>
#include "../../../inc/header.h"

bool is_user_online(int user_id) { // TODO: when clients sorting is implemented, add a binary search
    pthread_mutex_lock(&vendor.server.client_settings.clients_mutex);
    bool online = false;
    for (unsigned int i = 0; i < vendor.server.client_settings.max_clients; i++) {
        if (vendor.server.client_settings.clients[i].user_id == user_id) {
            online = true;
            break;
        }
    }
    pthread_mutex_unlock(&vendor.server.client_settings.clients_mutex);
    return online;
}



