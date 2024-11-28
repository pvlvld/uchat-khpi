#ifndef CHAT_H
#define CHAT_H

#include "header.h"

#define PER_PAGE 20

typedef struct {
    GtkWidget *(*init)(void);
    GtkWidget *(*no_chat_init)(void);
    void (*change_chat)(void);
    GtkWidget *chat_box;
    GtkWidget *scrolled_window;
    GtkWidget *content_box;
    GtkWidget *stretchable_box_old_messages;
    GtkWidget *stretchable_box_new_messages;
    GtkAdjustment *vadjustment;
    int total_messages;
    int temp_message_counter;
    int page;
    int shown_messages;
    int chat_id;
} t_chat;

typedef struct {
    GtkAdjustment *adjustment;
    double height;
} t_scroll_data;

typedef struct {
    GtkWidget *widget;
    int *width;
    int *height;
} t_widget_size_data;

typedef struct {
    GtkWidget *widget;
    int is_new;
    int chat_id;
    int message_id;
} t_message_info_struct;

t_chat init_chat(void);
void add_chat_message(t_messages_struct *message, int is_received);
GtkWidget *chat_create_scrolled_window(void);
GtkWidget *create_message_input(void);
GtkWidget *create_message_box(const char *message_txt, t_message_info_struct *message_info_struct);
gboolean set_scroll_to_bottom(gpointer data);
#endif //CHAT_H
