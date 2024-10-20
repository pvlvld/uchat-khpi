#include "../../../inc/server/client.h"
#include "../../../inc/header.h"
#include "../../../inc/websocket.h"

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
        char *jwt_token = extract_bearer_token(buffer);

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
                       if (vendor.env.dev_mode) printf("jwt_verify.status: %d\n",jwt_verify.status);
                        if (jwt_verify.status == 1) {
                            cJSON *jwt_payload = jwt_verify.payload;
                            cJSON *phone_number_item = cJSON_GetObjectItem(jwt_payload, "phone_number");
                            if (cJSON_IsString(phone_number_item)) {
                                char *phone_number = phone_number_item->valuestring;
                                user_id = vendor.database.users_table.find_by_phone(phone_number);
                                free(phone_number);
                            }
                        } else {
                            cJSON_AddStringToObject(response_json, "message", "WebSocket connection without invalid JWT token");
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
                        free(jwt_token);
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

                   if (vendor.env.dev_mode) printf("WebSocket handshake with user_id: %ld\n", user_id);  // Debug message
                    vendor.server.client_settings.update_client_user_id(ssl, user_id);

                    cJSON_AddStringToObject(response_json, "message", "Connected succsess!");
                    cJSON_AddStringToObject(response_json, "status", "OK");

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
