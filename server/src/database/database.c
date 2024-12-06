#include "../../inc/database.h"
#include "../../inc/database/database_pool.h"

t_database database;

t_database *init_database(void) {
    init_postgres_pool(&database.pool);

    database.messages_table.send_message = send_message;
    database.messages_table.get_message = get_message;
    database.messages_table.delete_message = delete_message;
    database.messages_table.edit_message_text = edit_message_text;
    database.messages_table.get_message = get_message;
    database.messages_table.get_messages = get_messages;

    database.personal_chat_table.create_personal_chat = create_personal_chat;
    database.personal_chat_table.get_personal_chat = get_personal_chat;
    database.personal_chat_table.delete_personal_chat = delete_personal_chat;
    database.personal_chat_table.update_background_personal_chat = update_background_personal_chat;
    database.personal_chat_table.get_user_chats_personal = get_user_chats_personal;

    database.users_table.create_user = create_user;
    database.users_table.get_user_by_login = get_user_by_login;
    database.users_table.get_user_by_id = get_user_by_id;
    database.users_table.update_user_locale = update_user_locale;

    return &database;
}
