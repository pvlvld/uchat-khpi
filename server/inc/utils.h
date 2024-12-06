#ifndef UTILS_H
#define UTILS_H

// compatibility with c99 and c11 (strdup)
#define _POSIX_C_SOURCE 200809L

#include <stdbool.h>
#include "header.h"
#include <string.h>


typedef struct {
    int Success;
    int message_id;
    char timestamp[20]; // Timestamp as a string (format: "YYYY-MM-DD HH:MM:SS")
} MessageResult_t;

typedef struct {
    int Success;  // Indicates whether the operation succeeded (1 for success, 0 for failure)
    int sender_id;
} MessageSenderResult_t;

typedef struct {
    int Success;       // Indicates result (-1: not sender, 0: deletion failed, 1: success)
    int message_id;    // ID of the deleted message
    char timestamp[20]; // Timestamp of the successful deletion (format: "YYYY-MM-DD HH:MM:SS")
} deleteMessageResult_t;

#include "handlers/post_handlers/delete_message.h"

bool is_user_online(int user_id);

char *itoa(int value, char *buffer);
extern char *strdup (const char *__s)
    __THROW __attribute_malloc__ __nonnull ((1));

void get_current_timestamp(char **timestamp_ptr);

char *hash_password(const char *password);

char *get_sender_login_from_token(const char *request);
char *get_sender_id_from_token(const char *request);

bool is_valid_login(const char *login);
bool is_valid_user_id(const char *user_id);
bool is_valid_password(const char *password);
bool is_valid_public_key(const char *public_key_str);
bool is_valid_message_id(char *message_id);

bool is_valid_username(const char *username);
char *username_validation(const char *username, const char *user_login, PGconn *conn);

// send message
bool is_user_in_chat(PGconn *conn, int chat_id, int user_id);
const char *get_user_role_in_group(PGconn *conn, int chat_id, int user_id);
char *extract_recipient_login(cJSON *json);
char *extract_recipient_username(cJSON *json);
char *extract_user_id(cJSON *json);

int get_dm_recipient_id(PGconn *conn, int chat_id, int sender_id);
PGresult *get_group_recipients(PGconn *conn, int chat_id);

char *extract_chat_id(cJSON *json);
const char *get_chat_type_ptr(PGconn *conn, int chat_id);
bool is_valid_chat_id(const char *chat_id_str);

char *extract_message(cJSON *json);
char *extract_message_id(cJSON *json);
bool is_valid_message(const char *message);
bool is_valid_message_id(char *message_id);

MessageSenderResult_t get_message_sender(PGconn *conn, int chat_id, int message_id);

MessageResult_t store_message_return_message_info(PGconn *conn, int chat_id, int sender_id, const char *message_text,
                                                int media_id, int reply_to_chat, int reply_to_message,
                                                int forwarded_from_chat, int forwarded_from_message);
MessageResult_t handle_personal_chat_message(PGconn *conn, int chat_id, int sender_id, int recipient_id, const char *message_text,
                                  int media_id, int reply_to_chat, int reply_to_message, int forwarded_from_chat, int forwarded_from_message);
MessageResult_t handle_group_or_channel_message(PGconn *conn, int chat_id, int sender_id, const char *message_text,
                                    int media_id, int reply_to_chat, int reply_to_message, int forwarded_from_chat, int forwarded_from_message);

cJSON *create_message_json(int sender_id, const char *message_text);
cJSON *create_delete_message_json(int sender_id, deleteMessageResult_t delete_result);



#endif // UTILS_H
