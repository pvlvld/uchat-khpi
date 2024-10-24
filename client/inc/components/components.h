#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "../header.h"
#include "input.h"

typedef struct {
    t_input input;
} t_components;

t_components init_components(void);

#endif //COMPONENTS_H
