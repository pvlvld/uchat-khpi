#include "../../../inc/header.h"
#include <gtk/gtk.h>

t_components init_components(void) {
    t_components components;

    components.input.create = create_input;

    return components;
}
