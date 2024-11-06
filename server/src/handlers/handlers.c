#include "../../inc/header.h"

t_handlers init_handlers(void) {
    t_middleware middleware = {
        ._jwt = jwt_middleware,
    };

    t_post_handler post_handlers = {
        .post = post_rout,
        ._login = login_rout,
        ._register = register_rout,
        .middleware = middleware,
    };

    t_get_handler get_handlers = {
        .secret = secret_rout,
        ._hello = hello_rout,
        .middleware = middleware,
    };

    t_handlers handlers = {
       .post = post_handlers,
       .get = get_handlers
    };
    return handlers;
}
