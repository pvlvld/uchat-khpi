#ifndef SERVER_REQUESTS_H
#define SERVER_REQUESTS_H

typedef struct {
    int (*fetch_data)(const char *endpoint);
    int (*post_data)(const char *endpoint, const char *payload);
    int (*delete_data)(const char *endpoint);
} t_server_requests;

t_server_requests init_server_requests(void);

#endif // SERVER_REQUESTS_H
