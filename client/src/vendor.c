#include "../inc/header.h"

t_vendor vendor;

void init_vendor(t_vendor *vendor) {
    vendor->database = init_database();
    vendor->crypto = init_crypto();
    vendor->components = init_components();
    vendor->modal = init_modal();
    vendor->helpers = init_helpers();
    vendor->pages = init_pages();
    vendor->window_content = NULL;
    vendor->debug_mode = 1;
    vendor->popup = init_popup();
    vendor->ssl_struct = init_ssl_struct();
}
