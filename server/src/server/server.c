#include "../../inc/server/server.h"
#include "../../inc/header.h"
#include "../../inc/ssl.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <unistd.h>

void start_server(void) {

    pid_t pid, sid;

    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        printf("Daemon PID: %d\n", pid);
        exit(EXIT_SUCCESS);
    }

    sid = setsid();
    if (sid < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    if (chdir("/") < 0) {
        perror("chdir failed");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDONLY); // STDIN
    open("/dev/null", O_WRONLY); // STDOUT
    open("/dev/null", O_WRONLY); // STDERR

    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t thread_id;

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(vendor.server.port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Server listening on port %d", 8080);

    while (1) {
        // Accept incoming connection
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Create a new thread to process the client
        if (pthread_create(&thread_id, NULL, vendor.server.client_settings.handle_client,
                           (void *)(intptr_t)client_socket) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        pthread_detach(thread_id);
    }
}

t_server init_server(void) {
    t_server server = {
        .port = vendor.env.port,
        .ssl_ctx = init_ssl_context(),
        .client_settings = init_client_settings(),
        .https = init_https(),
    };
    return server;
}
