#include "../../inc/header.h"

t_handlers init_handlers(void) {
    t_middleware middleware = {
        ._jwt = jwt_middleware,
    };

    t_post_handler post_handlers = {
        .post = post_rout,
        ._login = login_rout,
        ._register = register_rout,
        ._send_message = protected_send_message_rout,
        ._delete_message = protected_delete_message_rout,
        ._edit_message = protected_edit_message_rout,
        ._friend_request = protected_friend_request_rout,
        ._delete_friend = protected_delete_friend_rout,
        .middleware = middleware,
    };

    t_get_handler get_handlers = {
        .secret = secret_rout,
        ._hello = hello_rout,
        ._get_all_updates = protected_get_all_updates_rout,
        .middleware = middleware,
    };

    t_handlers handlers = {
       .post = post_handlers,
       .get = get_handlers
    };
    return handlers;
}
