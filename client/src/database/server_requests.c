#include "../../inc/header.h"

static int fetch_data_from_server(const char *endpoint) {
    printf("Fetching data from server: %s\n", endpoint);
    return 1;
}

static int post_data_to_server(const char *endpoint, const char *payload) {
    printf("Posting data to server: %s\nPayload: %s\n", endpoint, payload);
    return 1;
}

static int delete_data_on_server(const char *endpoint) {
    printf("Deleting data on server: %s\n", endpoint);
    return 1;
}

t_server_requests init_server_requests(void) {
    t_server_requests requests = {
        .fetch_data = fetch_data_from_server,
        .post_data = post_data_to_server,
        .delete_data = delete_data_on_server,
    };
    return requests;
}
