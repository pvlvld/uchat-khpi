#include "../../inc/header.h"

static int is_valid_ip(const char *ip) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip, &(sa.sin_addr)) != 0;
}

static int is_valid_port(const char *port_str) {
    int port = atoi(port_str);
    if (port <= 0 || port > 65535) return 0;

    for (int i = 0; port_str[i] != '\0'; i++) {
        if (!isdigit(port_str[i])) return 0;
    }
    return 1;
}

int init_server(int argc, char **argv) {
    (void) argc;
    (void) argv;
    (void) is_valid_ip;
    (void) is_valid_port;
//    if (argc != 3) {
//        printf("Usage: ./client <server address> <server port>\n");
//        return -1;
//    }
//
//    char *server_address = argv[1];
//    char *server_port = argv[2];
//
//    if (!is_valid_ip(server_address)) {
//        printf("Error: Invalid IP address or domain.\n");
//        return -1;
//    }
//
//    if (!is_valid_port(server_port)) {
//        printf("Error: Port must be a number between 1 and 65535.\n");
//        return -1;
//    }
//
//    vendor.server.address = server_address;
//    vendor.server.port = atoi(server_port);

    vendor.server.address = "127.0.0.1";
    vendor.server.port = 8080;

    return 1;
}
