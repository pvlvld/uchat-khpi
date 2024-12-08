#ifndef API_H
#define API_H

#include "header.h"

typedef struct {
  int chat_id;
  int sender_id;
  int message_id;
  char *message_encrypted;
  time_t timestamp;
} t_api_message_struct;

void friend_request_handler(cJSON *req);
gboolean new_message_handler(gpointer user_data);

#endif //API_H
