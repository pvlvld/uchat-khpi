#ifndef LOGIN_H
#define LOGIN_H

#include "header.h"

typedef struct {
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *toggle_button;
    GtkWidget *toggle_image;
} t_login_form_data;

typedef struct {
    GtkWidget *(*init)(void);
    t_login_form_data form_data;
    GtkWidget *username_wrapper;
    GtkWidget *password_wrapper;
    GtkWidget *username_error;
    GtkWidget *password_error;
} t_login_page;

GtkWidget *login_init(void);
t_login_page init_login(void);

#endif //LOGIN_H
