#include "../inc/header.h"

t_vendor vendor;

void init_vendor(t_vendor *vendor) {
    vendor->database = init_database();
    vendor->crypto = init_crypto();
    vendor->components = init_components();
    vendor->sidebar = init_sidebar();
    vendor->modal = init_modal();
    vendor->helpers = init_helpers();
    vendor->pages = init_pages();
    vendor->window_content = NULL;
    t_user current_user = {
        .id = 0,
        .username = "Admin",
        .user_login = "@admin",
        .profile_picture = "person_img.jpg"
    };
    vendor->current_user = current_user;
    vendor->popup = init_popup();
}
