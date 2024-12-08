#include <libpq-fe.h>
#include <stdbool.h>

// Check if user is in the specified chat (for both personal and group chats)
bool is_user_in_chat(PGconn *conn, int chat_id, int user_id) {
    const char *query = "SELECT 1 FROM personal_chats WHERE chat_id = $1 AND (user1_id = $2 OR user2_id = $2)";
    const char *params[2];
    char chat_id_str[12], user_id_str[12];

    snprintf(chat_id_str, sizeof(chat_id_str), "%d", chat_id);
    snprintf(user_id_str, sizeof(user_id_str), "%d", user_id);

    params[0] = chat_id_str;
    params[1] = user_id_str;

    PGresult *res = PQexecParams(conn, query, 2, NULL, params, NULL, NULL, 0);

    bool user_in_chat = (PQntuples(res) > 0);
    PQclear(res);

    if (!user_in_chat) {
        // Check if the user is in a group chat as well
        const char *group_query = "SELECT 1 FROM group_chat_members WHERE chat_id = $1 AND user_id = $2";
        res = PQexecParams(conn, group_query, 2, NULL, params, NULL, NULL, 0);
        user_in_chat = (PQntuples(res) > 0);
        PQclear(res);
    }

    return user_in_chat;
}
