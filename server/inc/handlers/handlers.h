#ifndef HANDLERS_H
#define HANDLERS_H

#include "../header.h"
#include "middleware.h"
#include "post.h"
#include "get.h"

typedef struct {
    t_post_handler post;
    t_get_handler get;
} t_handlers;

t_handlers init_handlers(void);

#endif //HANDLERS_H
