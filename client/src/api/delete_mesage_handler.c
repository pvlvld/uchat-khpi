#include "../../inc/header.h"
#include <sys/time.h>

gboolean delete_message_handler(gpointer user_data) {
    cJSON *req = (cJSON *)user_data;
    t_api_message_struct *api_message = malloc(sizeof(t_api_message_struct));
    api_message->chat_id = cJSON_GetObjectItem(req, "chat_id")->valueint;
    api_message->sender_id = cJSON_GetObjectItem(req, "sender_id")->valueint;
    api_message->message_id = cJSON_GetObjectItem(req, "message_id")->valueint;

    if (vendor.active_chat.chat->id == (unsigned int) api_message->chat_id) {
        GtkWidget *widget = get_chat_widget(api_message->message_id);

        gtk_widget_destroy(widget);
    }

    vendor.modal.message_info.destroy();
    vendor.database.tables.messages_table.delete_message(api_message->message_id, api_message->chat_id);

    if (vendor.active_chat.chat->last_message->message_id == api_message->message_id
        && vendor.active_chat.chat->id == (unsigned int) api_message->chat_id) {
        vendor.active_chat.chat->last_message = vendor.database.tables.messages_table.get_messages_by_chat_id(api_message->chat_id, 1, 1, NULL);
        update_chatblock(vendor.active_chat.chat_sidebar_widget, vendor.active_chat.chat, 0);
    }

    return FALSE;
}
