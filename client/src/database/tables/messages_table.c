#include "../../../inc/header.h"

static void create_table(void) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS messages ("
        "message_id INTEGER PRIMARY KEY, "
        "chat_id INTEGER NOT NULL, "
        "sender_id INTEGER NOT NULL, "
        "message_text TEXT NOT NULL, "
        "timestamp TIMESTAMP DEFAULT (strftime('%s', 'now')), "
        "edited_at TIMESTAMP DEFAULT NULL, "
        "read_at TIMESTAMP DEFAULT NULL, "
        "FOREIGN KEY (chat_id) REFERENCES chats(chat_id) ON DELETE CASCADE, "
        "FOREIGN KEY (sender_id) REFERENCES users(user_id) ON DELETE CASCADE"
        ");";

    vendor.database.sql.execute_sql(sql);
}

static t_messages_struct *add_message(int message_id, int chat_id, int sender_id, const char *message_text) {
    // Рассчитываем длину строки запроса
    size_t sql_size = snprintf(NULL, 0,
             "INSERT INTO messages (message_id, chat_id, sender_id, message_text) "
             "VALUES (%d, %d, %d, '%s');", message_id, chat_id, sender_id, message_text) + 1; // +1 для null-терминатора

    // Выделяем память для строки
    char *sql = malloc(sql_size);
    if (sql == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Формируем SQL запрос
    snprintf(sql, sql_size,
             "INSERT INTO messages (message_id, chat_id, sender_id, message_text) "
             "VALUES (%d, %d, %d, '%s');", message_id, chat_id, sender_id, message_text);

    vendor.database.sql.execute_sql(sql);

    // Создаем структуру для возврата
    t_messages_struct *message = malloc(sizeof(t_messages_struct));
    if (message == NULL) {
        fprintf(stderr, "Memory allocation failed for message structure\n");
        return NULL;
    }

    // Заполняем структуру
    message->message_id = message_id;
    message->chat_struct = vendor.database.tables.chats_table.get_chat_by_id(chat_id); // Получаем данные о чате
    message->sender_struct = vendor.database.tables.users_table.get_user_by_id(sender_id); // Получаем данные об отправителе
    char *decrypt = vendor.crypto.decrypt_data_from_db(message_text);
    if (decrypt) {
        message->message_text = vendor.helpers.strdup(decrypt);
        free(decrypt);
    }

    // Устанавливаем временную метку (например, текущее время)
    time_t current_time = time(NULL);
    localtime_r(&current_time, &message->timestamp);

    // Поля, которые отсутствуют при добавлении
    memset(&message->read_at, 0, sizeof(message->read_at));
    memset(&message->edited_at, 0, sizeof(message->edited_at));

    return message;
}


static void edit_message(int message_id, const char *new_message_text) {
    char sql[1024];
    snprintf(sql, sizeof(sql),
             "UPDATE messages SET message_text = '%s' WHERE message_id = %d;",
             new_message_text, message_id);

    vendor.database.sql.execute_sql(sql);
}

static t_messages_struct *get_message_by_chat_id_and_message_id(int chat_id, int message_id) {
    // Строка SQL-запроса для получения одного сообщения
    char sql[1024];
    snprintf(sql, sizeof(sql),
             "SELECT message_id, chat_id, sender_id, message_text, timestamp, read_at, edited_at "
             "FROM messages WHERE chat_id = %d AND message_id = %d;", chat_id, message_id);

    // Выполнение запроса
    char **results = NULL;
    int rows, cols;
    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    if (rc != 0 || rows == 0) {
        printf("Message not found for chat_id: %d, message_id: %d\n", chat_id, message_id);
        return NULL;  // Сообщение не найдено
    }

    // Выделяем память для структуры одного сообщения
    t_messages_struct *message = malloc(sizeof(t_messages_struct));
    if (message == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    // Заполняем структуру для одного сообщения
    message->message_id = atoi(results[cols * 1]);  // message_id
    message->chat_struct = vendor.database.tables.chats_table.get_chat_by_id(chat_id); // Данные о чате
    message->sender_struct = vendor.database.tables.users_table.get_user_by_id(atoi(results[cols * 1 + 2])); // Данные об отправителе

    // Дешифруем текст сообщения, если он есть
    char *decrypt = vendor.crypto.decrypt_data_from_db(results[cols * 1 + 3]);
    if (decrypt) {
        message->message_text = vendor.helpers.strdup(decrypt);
        free(decrypt);
    }

    // Читаем временную метку
    time_t timestamp = (time_t)(atoll(results[cols * 1 + 4]));
    localtime_r(&timestamp, &message->timestamp);

    // Читаем read_at, если оно существует
    if (results[cols * 1 + 5] != NULL) {
        time_t read_at_timestamp = (time_t)(atoll(results[cols * 1 + 5]));
        localtime_r(&read_at_timestamp, &message->read_at);
    } else {
        memset(&message->read_at, 0, sizeof(message->read_at));  // Если read_at не существует, очищаем
    }

    // Заполняем поле edited_at
    if (results[cols * 1 + 6] != NULL) {
        time_t edited_at_timestamp = (time_t)(atoll(results[cols * 1 + 6]));
        localtime_r(&edited_at_timestamp, &message->edited_at);
    }

    return message;
}


static t_messages_struct *get_messages_by_chat_id(int chat_id, int number_of_elements, int page, int *total_messages) {
    int offset = (page - 1) * number_of_elements;

    char sql[1024];
    snprintf(sql, sizeof(sql),
             "SELECT message_id, chat_id, sender_id, message_text, timestamp, read_at, edited_at "
             "FROM messages WHERE chat_id = %d "
             "ORDER BY timestamp DESC "
             "LIMIT %d OFFSET %d;", chat_id, number_of_elements, offset);

    char **results = NULL;
    int rows, cols;
    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    if (rc != 0 || rows == 0) {
        printf("No messages found for chat_id: %d\n", chat_id);
        return NULL;
    }

    if (total_messages != NULL) {
        char count_sql[512];
        snprintf(count_sql, sizeof(count_sql),
                 "SELECT COUNT(*) FROM messages WHERE chat_id = %d;", chat_id);

        char **count_results = NULL;
        int count_rows, count_cols;
        rc = vendor.database.sql.execute_query(count_sql, &count_results, &count_rows, &count_cols);

        if (rc == 0 && count_rows > 0) {
            *total_messages = atoi(count_results[1]);
        }
    }

    t_messages_struct *messages = malloc(sizeof(t_messages_struct) * rows);
    if (messages == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    for (int i = 1; i <= rows; ++i) {
        t_messages_struct *msg = &messages[i - 1];

        msg->message_id = atoi(results[i * cols]);  // message_id
        msg->chat_struct = vendor.database.tables.chats_table.get_chat_by_id(chat_id);
        msg->sender_struct = vendor.database.tables.users_table.get_user_by_id(atoi(results[i * cols + 2])); // sender struct
//        test1234
        char *decrypt = vendor.crypto.decrypt_data_from_db(results[i * cols + 3]);
        if (decrypt) {
            msg->message_text = vendor.helpers.strdup(decrypt);
            free(decrypt);
        }
//        msg->message_text = vendor.helpers.strdup(results[i * cols + 3]);  // message_text

        time_t timestamp = (time_t)(atoll(results[i * cols + 4]));
        localtime_r(&timestamp, &msg->timestamp);

        if (results[i * cols + 5] != NULL) {
            time_t read_at_timestamp = (time_t)(atoll(results[i * cols + 5]));
            localtime_r(&read_at_timestamp, &msg->read_at);
        }
    }

    return messages;
}

static void free_struct(t_messages_struct *message) {
    if (message != NULL) {
        if (message->chat_struct != NULL) {
            vendor.database.tables.chats_table.free_struct(message->chat_struct);
        }
        if (message->sender_struct != NULL) {
            vendor.database.tables.users_table.free_struct(message->sender_struct);
        }

        if (message->message_text != NULL) {
            free(message->message_text);
        }

        free(message);
    }
}

static int get_total_messages(void) {
    const char *sql = "SELECT COUNT(*) FROM messages;";
    char **results = NULL;
    int rows, cols;

    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    if (rc != 0 || rows == 0 || cols == 0) {
        fprintf(stderr, "Failed to fetch total messages count: %s\n", sqlite3_errmsg(vendor.database.db));
        return -1;
    }

    int total_messages = atoi(results[1]);

    return total_messages;
}

static int delete_message(int message_id, int chat_id) {
    char sql[1024];
    snprintf(sql, sizeof(sql),
             "DELETE FROM messages WHERE message_id = %d AND chat_id = %d;",
             message_id, chat_id);

    vendor.database.sql.execute_sql(sql);

    return 0;
}

static t_messages_struct *edit_message_and_get(int message_id, int chat_id, const char *new_message_text) {
    t_messages_struct *message_before = get_message_by_chat_id_and_message_id(chat_id, message_id);
    if (message_before == NULL) {
        return NULL;
    }

    time_t current_time = time(NULL);

    size_t sql_size = snprintf(NULL, 0,
             "UPDATE messages SET message_text = '%s', edited_at = %ld "
             "WHERE message_id = %d AND chat_id = %d;",
             new_message_text, (long) current_time, message_id, chat_id);

    char *sql = malloc(sql_size);
    if (sql == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    snprintf(sql, sql_size,
             "UPDATE messages SET message_text = '%s', edited_at = %ld "
             "WHERE message_id = %d AND chat_id = %d;",
             new_message_text, (long) current_time, message_id, chat_id);

    vendor.database.sql.execute_sql(sql);
    free(sql);

    t_messages_struct *message = malloc(sizeof(t_messages_struct));
    if (message == NULL) {
        fprintf(stderr, "Memory allocation failed for message structure\n");
        return NULL;
    }

    message->message_id = message_id;
    message->chat_struct = vendor.database.tables.chats_table.get_chat_by_id(chat_id);
    message->sender_struct = vendor.database.tables.users_table.get_user_by_id(chat_id);

    char *decrypt = vendor.crypto.decrypt_data_from_db(new_message_text);
    if (decrypt) {
        message->message_text = vendor.helpers.strdup(decrypt);
        free(decrypt);
    }

    message->timestamp = message_before->timestamp;
    message->edited_at = message_before->edited_at;

    memset(&message->read_at, 0, sizeof(message->read_at));

    return message;
}

t_messages_table init_messages_table(void) {
    t_messages_table table = {
        .create_table = create_table,
        .add_message = add_message,
        .edit_message = edit_message,
        .get_messages_by_chat_id = get_messages_by_chat_id,
        .free_struct = free_struct,
        .get_total_messages = get_total_messages,
        .delete_message = delete_message,
        .edit_message_and_get = edit_message_and_get,
    };

    return table;
}
