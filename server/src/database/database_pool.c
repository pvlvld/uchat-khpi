#include "../../inc/database/database_pool.h"

static Connection pool[POOL_SIZE];

// TODO: Take connection parameters from environment variables.
void init_postgres_pool_connections() {
    for (int i = 0; i < POOL_SIZE; i++) {
        pool[i].conn =
            PQconnectdb("user=your_username dbname=your_dbname password=your_password hostaddr=127.0.0.1 port=5432");
        if (PQstatus(pool[i].conn) != CONNECTION_OK) {
            fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(pool[i].conn));
            exit(1);
        }
        pool[i].in_use = 0;
    }
}

PGconn *acquire_connection() {
    pthread_mutex_lock(&pool_mutex);
    for (int i = 0; i < POOL_SIZE; i++) {
        if (!pool[i].in_use) {
            pool[i].in_use = 1;
            pthread_mutex_unlock(&pool_mutex);
            return pool[i].conn;
        }
    }
    pthread_mutex_unlock(&pool_mutex);
    return NULL;
}

void release_connection(PGconn *conn) {
    pthread_mutex_lock(&pool_mutex);
    for (int i = 0; i < POOL_SIZE; i++) {
        if (pool[i].conn == conn) {
            pool[i].in_use = 0;
            break;
        }
    }
    pthread_mutex_unlock(&pool_mutex);
}

void cleanup_pool() {
    for (int i = 0; i < POOL_SIZE; i++) PQfinish(pool[i].conn);
}
