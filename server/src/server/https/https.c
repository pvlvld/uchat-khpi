#include "../../../inc/server/https.h"

t_https init_https(void) {
    t_https https = {
        .handle_https_request = _handle_https_request,
        .send_https_response = _send_https_response
    };
    return https;
}
