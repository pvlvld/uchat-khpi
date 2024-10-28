#ifndef DATABASE_POOL_H
#define DATABASE_POOL_H
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <libpq-fe.h>
#include <stdbool.h>

#define POOL_SIZE 10

typedef struct {
    PGconn *conn;
    bool in_use;
} Connection;

static Connection pool[POOL_SIZE];
static pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_postgres_pool();
PGconn *acquire_connection();
void release_connection(PGconn *conn);
void cleanup_pool();

#endif // DATABASE_POOL_H