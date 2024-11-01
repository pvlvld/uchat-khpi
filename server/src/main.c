#include "../inc/header.h"
#include "../inc/server/server.h"

int main(void) {
    srand((unsigned int)time(NULL));

    // Structure initialization
    init_vendor(&vendor);

    // Initializing a database connections pool
    vendor.database.pool.init();

    // Creating a database connections in the pool
    vendor.database.pool.init_connections();

    start_server();

    free(vendor.server.client_settings.clients);
    SSL_CTX_free(vendor.server.ssl_ctx);
    return 0;
}
