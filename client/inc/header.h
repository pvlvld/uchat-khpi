#ifndef HEADER_H
#define HEADER_H

#pragma once

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <libnotify/notify.h>

typedef struct {
    unsigned int id;
    char *name;
    char *last_message;
    char *sender_name;
    int type;
    char *path_to_logo;
    int unreaded_messages;
    time_t timestamp;
} t_chat_info;

#include "program.h"
#include "components/components.h"
#include "sidebar.h"
#include "login.h"
#include "register.h"
#include "modal.h"
#include "helpers.h"
#include "pages.h"

typedef struct {
    t_chat_info *chat;
    GtkWidget* chat_sidebar_widget;
} t_active_chat;

typedef struct {
    t_chat_info *chat;
    GtkWidget* chat_sidebar_widget;
} t_hover_chat;

//typedef struct {
//
//} t_window;

typedef struct {
    t_components components;
    t_sidebar sidebar;
    t_active_chat active_chat;
    t_hover_chat hover_chat;
    t_modal modal;
    t_helpers helpers;
    t_pages pages;
    GtkWidget *window_content;
    GtkWidget *window;
} t_vendor;

extern t_vendor vendor;

t_chat_info **parse_chats_info(void);
void free_chats_info(t_chat_info **chats_info);
void init_vendor(t_vendor *vendor);

#endif //HEADER_H
