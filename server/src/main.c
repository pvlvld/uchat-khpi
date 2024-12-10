#include "../inc/header.h"
#include "../inc/server/server.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);
    if (port < 1024 || port > 65535) {
        fprintf(stderr, "Port must be in the range 1024-65535\n");
        exit(EXIT_FAILURE);
    }

    srand((unsigned int)time(NULL));

    // Structure initialization
    init_vendor(&vendor, port);

    // Initializing a database connections pool
    vendor.database.pool.init();

    // Creating a database connections in the pool
    vendor.database.pool.init_connections();

    start_server();

    free(vendor.server.client_settings.clients);
    SSL_CTX_free(vendor.server.ssl_ctx);
    return 0;
}
