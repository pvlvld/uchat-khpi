#include "../../../inc/header.h"
#include <gtk/gtk.h>

t_components init_components(void) {
    t_components components = {
        .input = {
            .create = create_input,
        },
        .title = {
            .create_h1 = create_title_h1,
        }
    };

    return components;
}
