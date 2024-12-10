#include "../../inc/header.h"

gboolean delete_friend_handler(gpointer user_data) {
    cJSON *req = (cJSON *)user_data;
    int chat_id = cJSON_GetObjectItem(req, "chat_id")->valueint;

    delete_chat_sidebar(vendor.pages.main_page.sidebar.widget, chat_id);
    vendor.modal.chat_info.destroy();
    vendor.database.tables.chats_table.delete_chat_and_related_data(chat_id, PERSONAL);

    return FALSE;
}
