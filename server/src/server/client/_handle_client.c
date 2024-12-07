#include "../../../inc/header.h"
#include "../../../inc/utils.h"
#include "../../../inc/server/client.h"
#include "../../../inc/websocket.h"

static char *extract_token_from_query(const char *request) {
    const char *token_key = "token=";
    char *token_start = strstr(request, token_key);
    if (token_start) {
        token_start += strlen(token_key);
        char *token_end = strchr(token_start, '&');
        if (!token_end) {
            token_end = strchr(token_start, ' ');
        }
        if (token_end) {
            size_t token_len = token_end - token_start;
            char *token = malloc(token_len + 1);
            if (token) {
                strncpy(token, token_start, token_len);
                token[token_len] = '\0';
                return token;
            }
        } else {
            // В случае если нет '&', но есть символ конца строки
            size_t token_len = strlen(token_start);
            char *token = malloc(token_len + 1);
            if (token) {
                strcpy(token, token_start);
                return token;
            }
        }
    }
    return NULL;
}


void *_handle_client(void *client_socket) {
    int sock = (intptr_t)client_socket;
    SSL *ssl;
    char buffer[BUFFER_SIZE] = {0};
    ssize_t read_size;

    ssl = SSL_new(vendor.server.ssl_ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        close(sock);
        return NULL;
    }

    t_client client = {sock, ssl, -1};
    vendor.server.client_settings.add_client(client);

    // Read the request from the client
    read_size = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (read_size > 0) {
        buffer[read_size] = '\0';
        if (vendor.env.dev_mode) printf("Received request:\n%s\n", buffer);

        // Extract JWT token from Authorization header
        char *jwt_token = extract_token_from_query(buffer);
        char *timestamp = extract_query_param(buffer, "timestamp");
        // If this is a WebSocket request
        if (strstr(buffer, "Upgrade: websocket") != NULL) {
            char *key_start = strstr(buffer, "Sec-WebSocket-Key: ");
            if (key_start) {
                key_start += strlen("Sec-WebSocket-Key: ");
                char *key_end = strchr(key_start, '\r');
                if (key_end) {
                    *key_end = '\0';

                    unsigned long user_id = 0;
                    vendor.websocket.handle_handshake(ssl, key_start);
                    cJSON *response_json = cJSON_CreateObject();

                    if (jwt_token) {
                        jwt_verification_result jwt_verify = vendor.jwt.verify_jwt_token(jwt_token);
                        if (vendor.env.dev_mode) printf("jwt_verify.status: %d\n", jwt_verify.status);
                        if (jwt_verify.status == 1) {
                            cJSON *jwt_payload = jwt_verify.payload;
                            cJSON *user_id_item = cJSON_GetObjectItem(jwt_payload, "id");
                            if (cJSON_IsNumber(user_id_item)) {
                                user_id = (unsigned long)user_id_item->valueint;
                            }
                        } else {
                            cJSON_AddStringToObject(response_json, "message",
                                                    "WebSocket connection without invalid JWT token");
                            cJSON_AddStringToObject(response_json, "status", "ERROR");
                            vendor.websocket.send_websocket_message(ssl, cJSON_Print(response_json));
                            vendor.websocket.send_close_frame(ssl, 1002); // 1002 - Protocol Error
                            vendor.server.client_settings.remove_client(ssl);
                            SSL_shutdown(ssl);
                            SSL_free(ssl);
                            close(sock);
                            cJSON_Delete(response_json);
                            return NULL;
                        }
                    } else {
                        cJSON_AddStringToObject(response_json, "message", "WebSocket connection without jwt");
                        cJSON_AddStringToObject(response_json, "status", "ERROR");
                        vendor.websocket.send_websocket_message(ssl, cJSON_Print(response_json));
                        vendor.websocket.send_close_frame(ssl, 1002); // 1002 - Protocol Error
                        vendor.server.client_settings.remove_client(ssl);
                        SSL_shutdown(ssl);
                        SSL_free(ssl);
                        close(sock);
                        cJSON_Delete(response_json);
                        return NULL;
                    }

                    // Retrieve userId from the request
                    if (user_id == 0) {
                        cJSON_AddStringToObject(response_json, "message", "Can't find this user");
                        cJSON_AddStringToObject(response_json, "status", "ERROR");
                        vendor.websocket.send_websocket_message(ssl, cJSON_Print(response_json));
                        vendor.websocket.send_close_frame(ssl, 1002); // 1002 - Protocol Error
                        vendor.server.client_settings.remove_client(ssl);
                        SSL_shutdown(ssl);
                        SSL_free(ssl);
                        close(sock);
                        cJSON_Delete(response_json);
                        return NULL;
                    }

                    if (vendor.env.dev_mode)
                        printf("WebSocket handshake with user_id: %ld\n", user_id); // Debug message
                    vendor.server.client_settings.update_client_user_id(ssl, user_id);

                    if (!timestamp) {
                        cJSON_AddStringToObject(response_json, "message", "Connected success!");
                        cJSON_AddStringToObject(response_json, "status", "OK");
                        cJSON_AddBoolToObject(response_json, "error", true);
                        cJSON_AddStringToObject(response_json, "error_code",
                                                "Timestamp not found. Cannot get updates.");
                    }
                    else {
                        char updates_request[512];
                        snprintf(updates_request, sizeof(updates_request),
                                 "GET /get_all_updates?token=%s&user_id=%lu&timestamp=%s HTTP/1.1\r\n\r\n", jwt_token,
                                 user_id, timestamp);

                        printf("Updates request: %s\n", updates_request);
                        cJSON *updates = get_all_updates(updates_request);

                        if (!updates) {
                            cJSON_AddStringToObject(response_json, "message", "Connected succsess!");
                            cJSON_AddStringToObject(response_json, "status", "OK");
                            cJSON_AddBoolToObject(response_json, "error", true);
                            cJSON_AddStringToObject(response_json, "error_code", "Failed to fetch updates.");
                        } else {
                            cJSON_AddStringToObject(response_json, "message", "Connected succsess!");
                            cJSON_AddStringToObject(response_json, "status", "OK");
                            cJSON_AddBoolToObject(response_json, "error", false);
                            cJSON_AddItemToObject(response_json, "updates", updates);
                        }
                    }
                    if (jwt_token) free(jwt_token);
                    vendor.websocket.send_websocket_message(ssl, cJSON_Print(response_json));

                    cJSON_Delete(response_json);
                    unsigned char frame[BUFFER_SIZE];
                    while (1) {
                        read_size = SSL_read(ssl, frame, sizeof(frame));
                        if (read_size <= 0) {
                            if (SSL_get_error(ssl, read_size) == SSL_ERROR_ZERO_RETURN) {
                                if (vendor.env.dev_mode) printf("Connection closed by the client\n");
                            } else {
                                perror("SSL_read");
                            }
                            break;
                        }
                        // WebSocket frame processing
                        vendor.websocket.process_websocket_frame(ssl, frame, read_size);
                    }
                }
            }
        } else {
            // Process HTTPS request
            vendor.server.https.handle_https_request(ssl, buffer);
        }
    } else {
        perror("SSL_read");
    }

    vendor.server.client_settings.remove_client(ssl);
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sock);

    return NULL;
}
