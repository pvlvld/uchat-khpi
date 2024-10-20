#include "../../inc/websocket.h"

t_websocket init_websocket(void) {
    t_websocket websocket = {
        .handle_handshake = _handle_handshake,
        .process_websocket_frame = _process_websocket_frame,
        .send_close_frame = _send_close_frame,
        .send_websocket_message = _send_websocket_message,
        .extract_from_ws = _extract_from_ws,
    };
    return websocket;
}
