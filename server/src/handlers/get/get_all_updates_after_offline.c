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
        vendor.database.pool.release_connection(conn);
        return updates;
    }

    // Fetch updates from the database
    if (!fetch_updates_from_database(conn, user_id, timestamp, updates)) {
        cJSON_AddBoolToObject(updates, "error", true);
        cJSON_AddStringToObject(updates, "error_code", "Failed to fetch updates");
        vendor.database.pool.release_connection(conn);
        return updates;
    }

    vendor.database.pool.release_connection(conn);

    cJSON_AddBoolToObject(updates, "error", false);
    return updates;
}

void get_all_updates_rout(SSL *ssl, const char *request) {
    cJSON *response_json = cJSON_CreateObject();

    // Extract the body from the HTTP request
    char *body = strstr(request, "\r\n\r\n");
    if (!body) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "NO_MESSAGE_BODY");
        cJSON_AddStringToObject(response_json, "message", "No message body found in the request.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    body += 4; // Skip the "\r\n\r\n" part
    cJSON *json = cJSON_Parse(body);
    if (!json) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "INVALID_JSON");
        cJSON_AddStringToObject(response_json, "message", "The provided JSON is invalid.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return;
    }

    // Extract the user ID, timestamp, and token
    char *user_id_str = extract_user_id(json);
    char *timestamp = extract_timestamp(json);
    char *jwt_token = get_sender_id_from_token(request);

    if (!user_id_str || !timestamp || !jwt_token) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "MISSING_PARAMETERS");
        cJSON_AddStringToObject(response_json, "message", "Missing user_id, timestamp, or token in the request.");
        vendor.server.https.send_https_response(ssl, "400 Bad Request", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);

        free(user_id_str);
        free(timestamp);
        free(jwt_token);
        return;
    }

    // Create a simulated GET request for get_all_updates
    char updates_request[512];
    snprintf(updates_request, sizeof(updates_request),
             "GET /get_all_updates?token=%s&user_id=%s&timestamp=%s HTTP/1.1\r\n\r\n",
             jwt_token, user_id_str, timestamp);

    // Call get_all_updates and handle the result
    cJSON *updates = get_all_updates(updates_request);
    if (!updates) {
        cJSON_AddBoolToObject(response_json, "error", true);
        cJSON_AddStringToObject(response_json, "code", "FAILED_UPDATES_FETCH");
        cJSON_AddStringToObject(response_json, "message", "Failed to fetch updates.");
        vendor.server.https.send_https_response(ssl, "500 Internal Server Error", "application/json", cJSON_Print(response_json));
        cJSON_Delete(response_json);

        free(user_id_str);
        free(timestamp);
        free(jwt_token);
        return;
    }

    // Add updates to the response JSON
    cJSON_AddBoolToObject(response_json, "error", false);
    cJSON_AddStringToObject(response_json, "code", "SUCCESS");
    cJSON_AddStringToObject(response_json, "message", "Updates fetched successfully.");
    cJSON_AddItemToObject(response_json, "updates", updates);

    // Send the final response
    vendor.server.https.send_https_response(ssl, "200 OK", "application/json", cJSON_Print(response_json));

    // Cleanup
    cJSON_Delete(response_json);
    free(user_id_str);
    free(timestamp);
    free(jwt_token);
}

void protected_get_all_updates_rout(SSL *ssl, const char *request) { jwt_middleware(ssl, request, get_all_updates_rout); }
