#ifndef HEADER_H
#define HEADER_H

#pragma once

#include "../../libraries/cJSON/cJSON.h"
#include "env.h"
#include "database.h"
#include "server/server.h"
#include "handlers/handlers.h"
#include "websocket.h"
#include "json_web_token.h"
#include <string.h>
#include <stdlib.h>

typedef struct {
    t_env env;
    t_database database;
    t_sql sql;
    void (*execute_sql)(const char *sql);
	t_server server;
	t_handlers handlers;
    t_websocket websocket;
    t_jwt jwt;
} t_vendor;

// Declaring a global variable
extern t_vendor vendor;

void init_vendor(t_vendor *vendor);

#endif //HEADER_H
