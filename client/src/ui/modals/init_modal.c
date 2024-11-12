#include "../../../inc/header.h"
#include <gtk/gtk.h>

t_modal init_modal(void) {
    t_modal modal = {
        .chat_info = init_modal_chat_info(),
        .profile_settings = init_modal_profile_settings(),
    };

    return modal;
}
