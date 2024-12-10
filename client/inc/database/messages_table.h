#ifndef MESSAGES_TABLE_H
#define MESSAGES_TABLE_H

#include "database.h"

typedef struct {
    int message_id;
    t_chats_struct *chat_struct;
    t_users_struct *sender_struct;
    char *message_text;
    char *path_to_image;
    struct tm timestamp;
    struct tm edited_at;
    struct tm read_at;
} t_messages_struct;

typedef struct {
    void (*create_table)(void);
    t_messages_struct *(*add_message)(int message_id, int chat_id, int sender_id, const char *message_text, const char *path_to_image, time_t timestamp);
    void (*edit_message)(int message_id, const char *new_message_text);
    t_messages_struct *(*get_messages_by_chat_id)(int chat_id, int number_of_elements, int page, int *total_messages);
    void (*free_struct)(t_messages_struct *message);
    int (*get_total_messages)(void);
    int (*delete_message)(int message_id, int chat_id);
    t_messages_struct *(*edit_message_and_get)(int message_id, int chat_id, const char *new_message_text);
} t_messages_table;
t_messages_table init_messages_table(void);

#endif //MESSAGES_TABLE_H
