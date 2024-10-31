#ifndef COMPONENTS_H
#define COMPONENTS_H

#pragma once

#include "../header.h"
#include "input.h"
#include "title.h"

typedef struct {
    t_input input;
    t_title title;
} t_components;

t_components init_components(void);

#endif //COMPONENTS_H
