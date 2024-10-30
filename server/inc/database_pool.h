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

struct DatabasePool {
    void (*init)();
    PGconn *(*acquire_connection)();
    void (*release_connection)(PGconn *);
    void (*cleanup)();
};

void init_postgres_pool();
PGconn *acquire_connection();
void release_connection(PGconn *conn);
void cleanup_pool();

static struct DatabasePool database_pool = {
    .init = init_postgres_pool,
    .acquire_connection = acquire_connection,
    .release_connection = release_connection,
    .cleanup = cleanup_pool
};


static Connection pool[POOL_SIZE];
static pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_postgres_pool();
PGconn *acquire_connection();
void release_connection(PGconn *conn);
void cleanup_pool();

#endif // DATABASE_POOL_H