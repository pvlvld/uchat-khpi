#ifndef PAGES_H
#define PAGES_H

#include "header.h"

typedef enum {
    LOGIN_PAGE,
    MAIN_PAGE,
    LOADING_PAGE,
} e_page_type;

typedef struct {
    e_page_type current_page;
    t_login_page login_page;
    GtkWidget *current_page_widget;
    void (*change_page)(e_page_type page);
} t_pages;

t_pages init_pages(void);
GtkWidget *create_main_page(void);
GtkWidget *create_login_page(void);
GtkWidget *create_loading_page(void);

#endif //PAGES_H
