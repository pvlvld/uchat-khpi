#include "../../inc/header.h"

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
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(vendor.server.port);
    addr.sin_addr.s_addr = inet_addr(vendor.server.address);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Connection failed");
        exit(1);
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
        perror("SSL_connect failed");
        exit(1);
    }

    return ssl;
}

cJSON* send_request(const char *method, const char *path, cJSON *json_body) {
    SSL *ssl = create_ssl_connection(create_socket());
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

t_ssl_struct init_ssl_struct(void) {
    t_ssl_struct ssl_struct = {
        .init_openssl = init_openssl,
        .cleanup_openssl = cleanup_openssl,
        .sock = create_socket(),
        .ssl = create_ssl_connection(ssl_struct.sock),
        .send_request = send_request,
    };
    return ssl_struct;
}
