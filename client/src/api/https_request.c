#include "../../inc/header.h"
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

void init_openssl(void) {
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_library_init();
}

void cleanup_openssl(void) {
    EVP_cleanup();
    ERR_free_strings();
}

int create_socket(void) {
    int sock;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(vendor.server.port);
    addr.sin_addr.s_addr = inet_addr(vendor.server.address);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }

    return sock;
}

SSL *create_ssl_connection(int sock) {
    SSL_CTX *ctx;
    SSL *ssl;

    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        perror("SSL_CTX_new failed");
        exit(1);
    }

    ssl = SSL_new(ctx);
    if (!ssl) {
        perror("SSL_new failed");
        exit(1);
    }

    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) != 1) {
        return NULL;
    }

    return ssl;
}

cJSON* send_request(const char *method, const char *path, cJSON *json_body) {
    SSL *ssl = create_ssl_connection(create_socket());
    if (!ssl) {
        return NULL;
    }
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE * 10] = {0};
    int response_len = 0;
    int bytes;
    int headers_end = 0;
    char *json_str = cJSON_PrintUnformatted(json_body);
    if (strcmp(method, "POST") == 0) {
        snprintf(request, sizeof(request),
                 "POST %s HTTP/1.1\r\n"
                 "Host: %s:%d\r\n"
                 "Content-Type: application/json\r\n"
                 "Content-Length: %zu\r\n"
                 "\r\n"
                 "%s",
                 path, vendor.server.address, vendor.server.port, strlen(json_str), json_str);
    } else if (strcmp(method, "GET") == 0) {
        snprintf(request, sizeof(request),
                 "GET %s HTTP/1.1\r\n"
                 "Host: %s:%d\r\n"
                 "\r\n",
                 path, vendor.server.address, vendor.server.port);
    } else {
        fprintf(stderr, "Unsupported HTTP method: %s\n", method);
        free(json_str);
        exit(1);
    }

    if (SSL_write(ssl, request, strlen(request)) <= 0) {
        perror("SSL_write failed");
        free(json_str);
        exit(1);
    }

    free(json_str);

    while ((bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';

        if (!headers_end) {
            char *header_end = strstr(buffer, "\r\n\r\n");
            if (header_end) {
                headers_end = 1;
                char *body_start = header_end + 4;
                strcat(response, body_start);
                response_len += strlen(body_start);
            }
        } else {
            strcat(response, buffer);
            response_len += bytes;
        }

        if (response_len > 0 && response[response_len - 1] == '}') {
            break;
        }
    }

    if (bytes < 0) {
        perror("SSL_read failed");
        exit(1);
    }

    cJSON *json_response = cJSON_Parse(response);
    if (!json_response) {
        fprintf(stderr, "Error parsing response as JSON: %s\n", cJSON_GetErrorPtr());
        exit(1);
    }

    return json_response;
}

void disconnect_websocket(void) {
    pthread_mutex_lock(&vendor.current_user.ws_client.lock);
    vendor.current_user.ws_client.running = 0;
    pthread_mutex_unlock(&vendor.current_user.ws_client.lock);
    printf("WebSocket disconnect requested.\n");
}

void *websocket_thread(void *arg) {
    (void) arg;
    char buffer[BUFFER_SIZE];
    int bytes;

    printf("WebSocket thread started.\n");

    while (1) {
        pthread_mutex_lock(&vendor.current_user.ws_client.lock);
        if (!vendor.current_user.ws_client.running) {
            pthread_mutex_unlock(&vendor.current_user.ws_client.lock);
            break;
        }
        pthread_mutex_unlock(&vendor.current_user.ws_client.lock);

        bytes = SSL_read(vendor.current_user.ws_client.ssl, buffer, sizeof(buffer) - 1);
        if (bytes > 0) {
            buffer[bytes] = '\0';

            char *json_start = strchr(buffer, '{');  // Locate the first JSON character
			if (json_start) {
				cJSON *json_message = cJSON_Parse(json_start);
    			if (json_message) {
        			printf("Parsed JSON: %s\n", cJSON_Print(json_message));
        			cJSON_Delete(json_message);
    			} else {
        			fprintf(stderr, "Failed to parse JSON: %s\n", cJSON_GetErrorPtr());
    			}
			} else {
    			fprintf(stderr, "No JSON detected in message: %s\n", buffer);
			}
        } else if (bytes == 0) {
            printf("Server closed connection.\n");
            break;
        } else {
            perror("SSL_read failed");
            break;
        }
    }

    printf("WebSocket thread exiting.\n");
    return NULL;
}

void *connect_websocket(void *arg) {
    (void) arg;
    SSL_CTX *ctx;
    char request[BUFFER_SIZE];
    int retry_interval = 5;

    while (vendor.current_user.ws_client.running) {
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();
        ctx = SSL_CTX_new(TLS_client_method());
        if (!ctx) {
            perror("SSL_CTX_new failed");
            return NULL;
        }

        vendor.current_user.ws_client.sock = socket(AF_INET, SOCK_STREAM, 0);
        if (vendor.current_user.ws_client.sock < 0) {
            perror("Socket creation failed");
            SSL_CTX_free(ctx);
            sleep(retry_interval);
            continue;
        }

        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(vendor.server.port);
        inet_pton(AF_INET, vendor.server.address, &server_addr.sin_addr);

        if (connect(vendor.current_user.ws_client.sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            close(vendor.current_user.ws_client.sock);
            SSL_CTX_free(ctx);
            sleep(retry_interval);
            continue;
        }

        vendor.current_user.ws_client.ssl = SSL_new(ctx);
        if (!vendor.current_user.ws_client.ssl) {
            perror("SSL_new failed");
            close(vendor.current_user.ws_client.sock);
            SSL_CTX_free(ctx);
            sleep(retry_interval);
            continue;
        }
        SSL_set_fd(vendor.current_user.ws_client.ssl, vendor.current_user.ws_client.sock);
        if (SSL_connect(vendor.current_user.ws_client.ssl) <= 0) {
            perror("SSL_connect failed");
            SSL_free(vendor.current_user.ws_client.ssl);
            close(vendor.current_user.ws_client.sock);
            SSL_CTX_free(ctx);
            sleep(retry_interval);
            continue;
        }

        snprintf(request, sizeof(request),
                 "GET /?token=%s HTTP/1.1\r\n"
                 "Host: %s:%d\r\n"
                 "Upgrade: websocket\r\n"
                 "Connection: Upgrade\r\n"
                 "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
                 "Sec-WebSocket-Version: 13\r\n"
                 "\r\n",
                 vendor.current_user.jwt, vendor.server.address, vendor.server.port);

        if (SSL_write(vendor.current_user.ws_client.ssl, request, strlen(request)) <= 0) {
            perror("SSL_write failed");
            close(vendor.current_user.ws_client.sock);
            SSL_free(vendor.current_user.ws_client.ssl);
            SSL_CTX_free(ctx);
            sleep(retry_interval);
            continue;
        }

        char buffer[BUFFER_SIZE];
        int bytes = SSL_read(vendor.current_user.ws_client.ssl, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) {
            perror("SSL_read failed");
            close(vendor.current_user.ws_client.sock);
            SSL_free(vendor.current_user.ws_client.ssl);
            SSL_CTX_free(ctx);
            sleep(retry_interval);
            continue;
        }

        buffer[bytes] = '\0';
        if (!strstr(buffer, "101 Switching Protocols")) {
            fprintf(stderr, "WebSocket handshake failed: %s\n", buffer);
            close(vendor.current_user.ws_client.sock);
            SSL_free(vendor.current_user.ws_client.ssl);
            SSL_CTX_free(ctx);
            sleep(retry_interval);
            continue;
        }

        printf("WebSocket connection established.\n");

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, websocket_thread, NULL) != 0) {
            perror("Failed to create thread");
            close(vendor.current_user.ws_client.sock);
            SSL_free(vendor.current_user.ws_client.ssl);
            SSL_CTX_free(ctx);
            sleep(retry_interval);
            continue;
        }

        pthread_join(thread_id, NULL);

        close(vendor.current_user.ws_client.sock);
        SSL_free(vendor.current_user.ws_client.ssl);
        SSL_CTX_free(ctx);

        printf("Reconnecting in %d seconds...\n", retry_interval);
        sleep(retry_interval);
    }

    printf("WebSocket client terminated.\n");
    return NULL;
}

t_ssl_struct init_ssl_struct(void) {
    t_ssl_struct ssl_struct = {
        .init_openssl = init_openssl,
        .cleanup_openssl = cleanup_openssl,
        .send_request = send_request,
    };
    return ssl_struct;
}
