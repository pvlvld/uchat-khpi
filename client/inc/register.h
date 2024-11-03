#ifndef REGISTER_H
#define REGISTER_H

#include "header.h"

typedef struct {
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *password_confirm_entry;
    GtkWidget *toggle_button;
    GtkWidget *toggle_image;
    GtkWidget *toggle_confirm_image;
} t_register_form_data;

typedef struct {
    gboolean (*on_key_press)(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
    gboolean (*on_from_entry_focus_in)(GtkWidget *entry, GdkEventFocus *event, gpointer user_data);
    gboolean (*on_from_entry_focus_out)(GtkWidget *entry, GdkEventFocus *event, gpointer user_data);
    void (*on_register_submit)(GtkButton *button, gpointer user_data);
    GtkWidget *(*create_username_wrapper)(void);
    GtkWidget *(*create_password_wrapper)(void);
    GtkWidget *(*create_password_confirm_wrapper)(void);
    GtkWidget *(*create_button)(void);
} t_register_functions;

typedef struct {
    GtkWidget *(*init)(void);
    t_register_form_data form_data;
    t_register_functions functions;
    GtkWidget *username_wrapper;
    GtkWidget *password_wrapper;
    GtkWidget *password_confirm_wrapper;
    GtkWidget *username_error;
    GtkWidget *password_error;
} t_register_page;

GtkWidget *create_register_username_wrapper(void);
GtkWidget *create_register_password_wrapper(void);
GtkWidget *create_register_password_confirm_wrapper(void);
GtkWidget *create_register_button(void);

GtkWidget *register_init(void);
t_register_page init_register(void);
t_register_functions init_register_functions(void);

#endif //REGISTER_H
