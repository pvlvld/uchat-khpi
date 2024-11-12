#include "../../../inc/database/tables/media_table.h"
#include "../../../inc/utils.h"
#include <stdlib.h>
#include <string.h>

static inline char *media_type_to_str(enum e_media_type media_type) {
    switch (media_type) {
    case IMAGE:
        return "image";
    case ANIMATION:
        return "animation";
    case AUDIO:
        return "audio";
    case VIDEO:
        return "video";
    default:
        return NULL;
    }
}

static inline enum e_media_type str_to_media_type(const char *media_type) {
    if (strcmp(media_type, "image") == 0) {
        return IMAGE;
    } else if (strcmp(media_type, "animation") == 0) {
        return ANIMATION;
    } else if (strcmp(media_type, "audio") == 0) {
        return AUDIO;
    } else if (strcmp(media_type, "video") == 0) {
        return VIDEO;
    } else {
        return -1;
    }
}

int create_media(PGconn *conn, enum e_media_type media_type, const char *content, int duration, int height, int width,
                 const char *hash_original, const char *hash_compressed) {
    const char *query =
        "INSERT INTO media (media_type, content, duration, height, width, hash_original, hash_compressed) VALUES "
        "($1::e_media_type, $2::bytea, $3::int, $4::int, $5::int, $6::varchar, $7::varchar) RETURNING media_id";

    const char *params[7] = {media_type_to_str(media_type),
                             content,
                             itoa(duration, NULL),
                             itoa(height, NULL),
                             itoa(width, NULL),
                             hash_original,
                             hash_compressed};

    if (params[0] == NULL) {
        fprintf(stderr, "Invalid media type\n");
        return -1;
    }

    PGresult *res = PQexecParams(conn, query, 7, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        int media_id = atoi(PQgetvalue(res, 0, 0));
        PQclear(res);
        return media_id;
    }

    fprintf(stderr, "Create media failed: %s\n", PQerrorMessage(conn));
    return -1;
}

bool get_media_by_id(PGconn *conn, int media_id, enum e_media_type *media_type, char **content, int *duration,
                     int *height, int *width, char **hash_original, char **hash_compressed) {
    const char *query = "SELECT media_type, content, duration, height, width, hash_original, hash_compressed "
                        "FROM media WHERE media_id = $1::int";

    char media_id_str[12];
    const char *params[1] = {itoa(media_id, media_id_str)};

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_TUPLES_OK) {
        *media_type = str_to_media_type(PQgetvalue(res, 0, 0));
        *content = PQgetvalue(res, 0, 1);
        *duration = atoi(PQgetvalue(res, 0, 2));
        *height = atoi(PQgetvalue(res, 0, 3));
        *width = atoi(PQgetvalue(res, 0, 4));
        *hash_original = PQgetvalue(res, 0, 5);
        *hash_compressed = PQgetvalue(res, 0, 6);
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Get media failed: %s\n", PQerrorMessage(conn));
    return false;
}

bool delete_media_by_id(PGconn *conn, int media_id) {
    const char *query = "DELETE FROM media WHERE media_id = $1::int";

    char media_id_str[12];
    const char *params[1] = {itoa(media_id, media_id_str)};

    PGresult *res = PQexecParams(conn, query, 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) == PGRES_COMMAND_OK) {
        PQclear(res);
        return true;
    }

    fprintf(stderr, "Delete media failed: %s\n", PQerrorMessage(conn));
    return false;
}
