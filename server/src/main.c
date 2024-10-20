#include "../inc/websocket.h"
#include "../inc/header.h"
#include "../inc/server/server.h"

int main(void) {
    srand((unsigned int)time(NULL));

    // Structure initialization
    init_vendor(&vendor);

    // Creating a database
    vendor.database.create_database();

    start_server();

    vendor.database.close_database();
    free(vendor.server.client_settings.clients);
    SSL_CTX_free(vendor.server.ssl_ctx);
    return 0;
}
