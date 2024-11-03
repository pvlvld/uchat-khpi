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
    gboolean (*on_key_press)(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
    gboolean (*on_from_entry_focus_in)(GtkWidget *entry, GdkEventFocus *event, gpointer user_data);
    gboolean (*on_from_entry_focus_out)(GtkWidget *entry, GdkEventFocus *event, gpointer user_data);
    void (*on_login_submit)(GtkButton *button, gpointer user_data);
    GtkWidget *(*create_username_wrapper)(void);
    GtkWidget *(*create_password_wrapper)(void);
    GtkWidget *(*create_button)(void);
} t_login_functions;

typedef struct {
    GtkWidget *(*init)(void);
    t_login_form_data form_data;
    t_login_functions functions;
    GtkWidget *username_wrapper;
    GtkWidget *password_wrapper;
    GtkWidget *username_error;
    GtkWidget *password_error;
} t_login_page;

GtkWidget *create_login_username_wrapper(void);
GtkWidget *create_login_password_wrapper(void);
GtkWidget *create_login_button(void);

GtkWidget *login_init(void);
t_login_page init_login(void);
t_login_functions init_login_functions(void);

#endif //LOGIN_H
