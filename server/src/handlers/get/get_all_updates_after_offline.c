#include "../../../inc/handlers/get_handlers/get_all_updates_after_offline.h"
#include <stdio.h>

char *extract_query_param(const char *request, const char *param_name) {
    if (!request || !param_name) { return NULL; }

    // Build the search key as "param_name="
    size_t key_len = strlen(param_name) + 1; // +1 for '='
    char *key = malloc(key_len + 1);
    if (!key) { return NULL; }
    snprintf(key, key_len + 1, "%s=", param_name);

    // Find the start of the parameter
    char *param_start = strstr(request, key);
    free(key);
    if (!param_start) { return NULL; }

    param_start += strlen(param_name) + 1; // Move past "param_name="

    // Find the end of the parameter value
    char *param_end = strchr(param_start, '&');
    if (!param_end) {
        param_end = strchr(param_start, ' '); // Check for end of request
    }

    // Extract the parameter value
    if (param_end) {
        size_t param_len = param_end - param_start;
        char *param_value = malloc(param_len + 1);
        if (param_value) {
            strncpy(param_value, param_start, param_len);
            param_value[param_len] = '\0';
            return param_value;
        }
    } else {
        // If no '&' or ' ', assume the parameter continues to the end of the string
        size_t param_len = strlen(param_start);
        char *param_value = malloc(param_len + 1);
        if (param_value) {
            strcpy(param_value, param_start);
            return param_value;
        }
    }

    return NULL;
}

cJSON *get_all_updates(const char *request) { //jwt_token=<jwt_token>&user_id=<user_id>&timestamp=<timestamp>
    cJSON *updates = cJSON_CreateObject();
    char *jwt_token = extract_query_param(request, "token");
    if (jwt_token) {
        jwt_verification_result jwt_verify = vendor.jwt.verify_jwt_token(jwt_token);
        if (!jwt_verify.status) {
            cJSON_AddBoolToObject(updates, "error", true);
            cJSON_AddStringToObject(updates, "error_code", "JWT token verification failed.");
            return updates;
        }
        } else {
        cJSON_AddBoolToObject(updates, "error", true);
        cJSON_AddStringToObject(updates, "error_code", "Missing JWT token.");
        return updates;
    }
    free(jwt_token);

    // Extract query parameters from the request
    char *user_id_str = extract_query_param(request, "user_id");
    char *timestamp_str = extract_query_param(request, "timestamp");

    if (!user_id_str || !timestamp_str) {
        cJSON_AddBoolToObject(updates, "error", true);
        cJSON_AddStringToObject(updates, "error_code", "Missing user_id or timestamp query parameters");
        free(user_id_str);
        free(timestamp_str);
        return updates;
    }

    if (!is_valid_user_id(user_id_str)) {
        cJSON_AddBoolToObject(updates, "error", true);
        cJSON_AddStringToObject(updates, "error_code", "Invalid user id.");
        free(user_id_str);
        free(timestamp_str);
        return updates;
    }
    int user_id = atoi(user_id_str);
    time_t timestamp = (time_t)atol(timestamp_str);
    free(user_id_str);
    free(timestamp_str);

    // Connect to the database
    PGconn *conn = vendor.database.pool.acquire_connection();
    if (PQstatus(conn) != CONNECTION_OK) {
        cJSON_AddBoolToObject(updates, "error", true);
        cJSON_AddStringToObject(updates, "error_code", "Database connection failed");
        PQfinish(conn);
        return updates;
    }

    // Fetch updates from the database
    if (!fetch_updates_from_database(conn, user_id, timestamp, updates)) {
        cJSON_AddBoolToObject(updates, "error", true);
        cJSON_AddStringToObject(updates, "error_code", "Failed to fetch updates");
        PQfinish(conn);
        return updates;
    }

    PQfinish(conn);

    cJSON_AddBoolToObject(updates, "error", false);
    return updates;
}
