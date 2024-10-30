#ifndef DATABASE_POOL_H
#define DATABASE_POOL_H
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <libpq-fe.h>
#include <stdbool.h>

// TODO: Make pool size configurable at runtime.
#define POOL_SIZE 10

typedef struct {
    PGconn *conn;
    bool in_use;
} Connection;

struct DatabasePool {
    /* A function pointer to initialize the database pool. */
    void (*init)();
    /* A function pointer to acquire a connection from the database pool. */
    PGconn *(*acquire_connection)();
    /* A function pointer to release a connection back to the database pool. */
    void (*release_connection)(PGconn *);
    /* A function pointer to close all connections in the database pool. */
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