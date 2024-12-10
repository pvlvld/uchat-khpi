#include "../../../../inc/handlers/post_handlers/edit_message.h"
#include <time.h>
#include <string.h>

void get_current_timestamp(char **timestamp_ptr) {
    time_t now = time(NULL);
    if (now == ((time_t)-1)) {
        *timestamp_ptr = NULL; // Set pointer to NULL if time() fails
        return;
    }

    struct tm *tm_info = localtime(&now);
    if (tm_info == NULL) {
        *timestamp_ptr = NULL; // Set pointer to NULL if localtime() fails
        return;
    }

    // Allocate memory for timestamp and format it
    *timestamp_ptr = malloc(20); // Allocate enough for "YYYY-MM-DD HH:MM:SS\0"
    if (*timestamp_ptr == NULL) {
        return; // Allocation failed, leave pointer as NULL
    }

    if (strftime(*timestamp_ptr, 20, "%Y-%m-%d %H:%M:%S", tm_info) == 0) {
        free(*timestamp_ptr); // Cleanup if strftime fails
        *timestamp_ptr = NULL;
    }
}



void is_user_allowed_to_edit(PGconn *conn, int chat_id, int req_sender_id, int message_id, EditMessageResult_t *edit_message_result) {
    const char *chat_type = get_chat_type_ptr(conn, chat_id);
    if (chat_type == NULL) {
        edit_message_result->Error = 1;
        edit_message_result->error_code = "UNKNOWN_CHAT_TYPE";
        return;
    }

    const char *user_role;
    if (strcmp(chat_type, "group") == 0 || strcmp(chat_type, "channel") == 0) {
        user_role = get_user_role_in_group(conn, chat_id, req_sender_id);
        if (user_role == NULL) {
            edit_message_result->Error = 1;
            edit_message_result->error_code = "USER_ROLE_NOT_FOUND";
            return;
        }
    } else if (strcmp(chat_type, "personal") == 0) {
        user_role = "personal_chat_user";
    } else {
        edit_message_result->Error = 1;
        edit_message_result->error_code = "UNKNOWN_CHAT_TYPE";
        return;
    }

    if (strcmp(chat_type, "group") == 0) {
        if (strcmp(user_role, "admin") == 0 || strcmp(user_role, "owner") == 0) {
            edit_message_result->Error = 0;
            return;
        } else if (strcmp(user_role, "member") == 0) {
                MessageSenderResult_t message_sender_result = get_message_sender(conn, chat_id, message_id);
                if (message_sender_result.Success == 0) {
                    edit_message_result->Error = 1;
                    edit_message_result->error_code = "MESSAGE_SENDER_NOT_FOUND";
                    return;
                }

                if (message_sender_result.sender_id == req_sender_id) {
                    edit_message_result->Error = 0;
                    return;
                } else {
                    edit_message_result->Error = 1;
                    edit_message_result->error_code = "USER_NOT_ALLOWED_TO_EDIT_THIS_MESSAGE";
                    return;
                }
        }


    } else if (strcmp(chat_type, "personal") == 0) {
        MessageSenderResult_t message_sender_result = get_message_sender(conn, chat_id, message_id);
        if (message_sender_result.Success == 0) {
            edit_message_result->Error = 1;
            edit_message_result->error_code = "MESSAGE_SENDER_NOT_FOUND";
            return;
        }

        if (message_sender_result.sender_id == req_sender_id) {
            edit_message_result->Error = 0;
            return;
        } else {
            edit_message_result->Error = 1;
            edit_message_result->error_code = "USER_NOT_ALLOWED_TO_EDIT_THIS_MESSAGE";
            return;
        }


    } else if (strcmp(chat_type, "channel") == 0) {
        if (strcmp(user_role, "admin") == 0 || strcmp(user_role, "owner") == 0) {
            edit_message_result->Error = 0;
            return;
        } else {
            edit_message_result->Error = 1;
            edit_message_result->error_code = "USER_NOT_ALLOWED_TO_EDIT_THIS_MESSAGE";
            return;
        }
    }
}

EditMessageResult_t perform_message_edit(PGconn *conn, int chat_id, int req_sender_id, int message_id, char *new_message_text_str) {
    EditMessageResult_t edit_message_result = {.Error = 0, .error_code = NULL, .message_id = -1, .timestamp = NULL, .new_message_text = NULL};
    is_user_allowed_to_edit(conn, chat_id, req_sender_id, message_id, &edit_message_result);
    if (edit_message_result.Error == 1) {
        return edit_message_result;
    }
    bool message_edit_db_res = edit_message_text(conn, chat_id, message_id, new_message_text_str);
    if (!message_edit_db_res) {
        edit_message_result.Error = 1;
        edit_message_result.error_code = "EDIT_MESSAGE_DB_ERROR";
        return edit_message_result;
    }

    edit_message_result.message_id = message_id;

    get_current_timestamp(&edit_message_result.timestamp);

    // Check if the timestamp pointer is NULL
    if (edit_message_result.timestamp == NULL) {
        edit_message_result.Error = 1;
        edit_message_result.error_code = "EDIT_MESSAGE_TIMESTAMP_ERROR";
        return edit_message_result;
    }
    edit_message_result.new_message_text = new_message_text_str;
    return edit_message_result;
}


