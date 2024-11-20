#ifndef HEADER_H
#define HEADER_H

#pragma once

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <libnotify/notify.h>
#include "../../libraries/sqlite/inc/sqlite3.h"
#include "../../libraries/cJSON/inc/cJSON.h"

#include "program.h"
#include "database/database.h"
#include "crypto.h"
#include "components/components.h"
#include "sidebar.h"
#include "chat.h"
#include "login.h"
#include "register.h"
#include "modal.h"
#include "helpers.h"
#include "pages.h"
#include "database/server_requests.h"
#include "database/edit_delete_layer.h"

typedef struct {
    t_chat_info *chat;
    GtkWidget* chat_sidebar_widget;
} t_active_chat;

typedef struct {
    t_chat_info *chat;
    GtkWidget* chat_sidebar_widget;
} t_hover_chat;

typedef struct {
    int user_id;
    char *username;
    char *user_login;
    char *about;
    char *password;
    char *profile_picture;
} t_user;

typedef struct {
    GtkWidget *window;
    GtkWidget *block;
    void (*add_message)(const char *message);
    void (*init)(void);
    void (*hide)(void);
    void (*show)(void);
    void (*update)(void);
} t_popup;
t_popup init_popup(void);

typedef struct {
    char *address;
    int port;
} t_server;

typedef struct {
    t_database database;
    t_crypto crypto;
    t_components components;
    t_sidebar sidebar;
    t_active_chat active_chat;
    t_hover_chat hover_chat;
    t_modal modal;
    t_helpers helpers;
    t_pages pages;
    GtkWidget *window_content;
    GtkWidget *window;
    t_popup popup;
    t_user current_user;
    t_server server;
    t_server_requests server_requests;
} t_vendor;

extern t_vendor vendor;

char *get_last_message_by_chat_id(int chat_id, int *sender_id);
int get_other_user_id(int chat_id);
char *get_group_name_by_chat_id(int chat_id);
char *get_user_name(int user_id);
t_chat_info **parse_chats_info(void);
void free_chats_info(t_chat_info **chats_info);
void init_vendor(t_vendor *vendor);
int init_server(int argc, char **argv);
char *strdup(const char *str);

#endif //HEADER_H
