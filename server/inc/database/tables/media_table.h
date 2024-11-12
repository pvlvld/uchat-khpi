#ifndef MEDIA_TABLE_H
#define MEDIA_TABLE_H
#pragma once

#include <libpq-fe.h>
#include <stdbool.h>

enum e_media_type {
    IMAGE,
    ANIMATION,
    AUDIO,
    VIDEO
};

int create_media(PGconn *conn, enum e_media_type media_type, const char *content, int duration, int height, int width,
                 const char *hash_original, const char *hash_compressed);

bool get_media_by_id(PGconn *conn, int media_id, enum e_media_type *media_type, char **content, int *duration,
                     int *height, int *width, char **hash_original, char **hash_compressed);

bool delete_media_by_id(PGconn *conn, int media_id);

typedef struct {
    int (*create_media)(PGconn *conn, enum e_media_type media_type, const char *content, int duration, int height, int width,
                        const char *hash_original, const char *hash_compressed);
    bool (*get_media_by_id)(PGconn *conn, int media_id, enum e_media_type *media_type, char **content, int *duration,
                            int *height, int *width, char **hash_original, char **hash_compressed);
    bool (*delete_media_by_id)(PGconn *conn, int media_id);
} t_media_table;

#endif // MEDIA_TABLE_H
