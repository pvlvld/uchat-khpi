#include "../../inc/database.h"

void init_database(t_database *database) {
    database->pool.init();

    database->messages_table.send_message = send_message;
    database->messages_table.get_message = get_message;
    database->messages_table.delete_message = delete_message;
    database->messages_table.edit_message_text = edit_message_text;
    database->messages_table.get_message = get_message;
    database->messages_table.get_messages = get_messages;

    database->personal_chat_table.create_personal_chat = create_personal_chat;
    database->personal_chat_table.get_personal_chat = get_personal_chat;
    database->personal_chat_table.delete_personal_chat = delete_personal_chat;
    database->personal_chat_table.update_background_personal_chat = update_background_personal_chat;
    database->personal_chat_table.list_user_personal_chats = list_user_personal_chats;

    database->users_table.create_user = create_user;
    database->users_table.get_user_by_login = get_user_by_login;
    database->users_table.get_user_by_id = get_user_by_id;
    database->users_table.update_user_locale = update_user_locale;
}
