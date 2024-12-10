#include "../inc/database.h"
#include "../inc/header.h"
#include "../inc/server/server.h"

t_vendor vendor;

void init_vendor(t_vendor *vendor, int port) {
    vendor->env = init_env(port);

    vendor->database = *init_database();
    vendor->server = init_server();
    vendor->handlers = init_handlers();
    vendor->websocket = init_websocket();
    vendor->jwt = init_jwt();
}
