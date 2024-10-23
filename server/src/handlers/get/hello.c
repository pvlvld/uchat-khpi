#include "../../../inc/header.h"

void hello_rout(SSL *ssl, const char *request) {
    vendor.server.https.send_https_response(ssl, "200 OK", "application/json", "{\"message\": \"hello from https\"}");

    if (!request) {
        return; // Заглушка щоб не було помилки
    }
}
