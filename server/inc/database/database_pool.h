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

typedef struct {
    /* A function pointer to initialize the database pool. */
    void (*init)();
    /* A function pointer to acquire a connection from the database pool. */
    PGconn *(*acquire_connection)();
    /* A function pointer to release a connection back to the database pool. */
    void (*release_connection)(PGconn *);
    /* A function pointer to close all connections in the database pool. */
    void (*cleanup)();
} t_database_pool;

void init_postgres_pool(t_database_pool *database_pool);
void init_postgres_pool_connections();
PGconn *acquire_connection();
void release_connection(PGconn *conn);
void cleanup_pool();

#endif // DATABASE_POOL_H