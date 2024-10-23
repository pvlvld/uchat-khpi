#include "../inc/database.h"
#include "../inc/header.h"
#include "../inc/server/server.h"
#include "../inc/ssl.h"

t_vendor vendor;

void init_vendor(t_vendor *vendor) {
    vendor->env = init_env();

    vendor->database = init_database();
    vendor->sql = sqlite;
    vendor->server = init_server();
    vendor->handlers = init_handlers();
    vendor->websocket = init_websocket();
    vendor->jwt = init_jwt();
}
