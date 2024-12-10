#include "../../inc/header.h"
#include <sys/time.h>

gboolean get_edited_message_handler(gpointer user_data) {
    cJSON *req = (cJSON *)user_data;
    t_message_info_struct *message_info = malloc(sizeof(t_api_message_struct));
    message_info->chat_id = cJSON_GetObjectItem(req, "chat_id")->valueint;
    message_info->sender_id = cJSON_GetObjectItem(req, "sender_id")->valueint;
    message_info->is_new = 1;

    cJSON *message_text_json = cJSON_GetObjectItem(req, "new_message_text");

    if (message_info->sender_id == vendor.current_user.user_id) {
        return FALSE;
    }

    message_info->message_id = cJSON_GetObjectItem(req, "message_id")->valueint;
    char *message = NULL;

    if (message_text_json != NULL) {
        message = message_text_json->valuestring;
    } else {
        message = "Edited message";
    }
    char *decrypted_message = message;

    if (vendor.database.tables.chats_table.get_chat_by_id(message_info->chat_id)->chat_type == PERSONAL && message_text_json != NULL) {
        decrypted_message = vendor.helpers.strdup(vendor.crypto.decrypt_data_from_db(message));
    }

    char *encrypt = vendor.crypto.encrypt_data_for_db(vendor.crypto.public_key_str, decrypted_message);

   if (encrypt != NULL) {
       t_messages_struct *message_struct = vendor.database.tables.messages_table.edit_message_and_get(message_info->message_id, message_info->chat_id, encrypt);

       if (vendor.active_chat.chat->id == (unsigned int) message_info->chat_id) {
           GtkWidget *widget = get_chat_widget(message_info->message_id);
           if (widget) {
               message_info->widget = widget;
               redraw_message_wrapper(message_info, message_struct);
           } else {
               g_warning("Widget for chat_id %d not found.", message_info->chat_id);
           }
       }

       if (vendor.active_chat.chat->last_message->message_id == message_struct->message_id
           && vendor.active_chat.chat->id == (unsigned int) message_struct->chat_struct->chat_id) {
           vendor.active_chat.chat->last_message = message_struct;
           update_chatblock(vendor.active_chat.chat_sidebar_widget, vendor.active_chat.chat, 0);
           }
       free(encrypt);
   }

    return FALSE;
}
