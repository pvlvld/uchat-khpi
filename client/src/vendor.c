#include "../inc/header.h"

t_vendor vendor;

void init_vendor(t_vendor *vendor) {
    vendor->components = init_components();
    vendor->sidebar = init_sidebar();
    vendor->modal = init_modal();
    vendor->helpers = init_helpers();
}
