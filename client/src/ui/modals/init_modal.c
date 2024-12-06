#include "../../../inc/header.h"
#include <gtk/gtk.h>

t_modal init_modal(void) {
    t_modal modal = {
        .chat_info = init_modal_chat_info(),
        .message_info = init_modal_message_info(),
        .profile_settings = init_modal_profile_settings(),
        .create_group = init_modal_create_group(),
        .add_users_to_group = init_modal_add_users_to_group(),
    };

    return modal;
}
