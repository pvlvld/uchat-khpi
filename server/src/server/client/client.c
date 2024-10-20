#include "../../../inc/server/client.h"
#include "../../../inc/header.h"

t_client_settings init_client_settings(void) {
    t_client_settings client_settings = {
		.clients_mutex = PTHREAD_MUTEX_INITIALIZER,
		.max_clients = atoi(getenv("MAX_CLIENTS") ? getenv("MAX_CLIENTS") : "256"),
		.client_count = 0,
		.clients = calloc(client_settings.max_clients, sizeof(t_client)),
        .handle_client = _handle_client,
        .add_client = _add_client,
        .update_client_user_id = _update_client_user_id,
        .remove_client = _remove_client,
        .send_message_to_client = _send_message_to_client
    };
    return client_settings;
}
