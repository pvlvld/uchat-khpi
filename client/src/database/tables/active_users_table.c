#include "../../../inc/header.h"

static void create_active_users_table(void) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS active_users ("
        "user_id INTEGER PRIMARY KEY NOT NULL, "
        "username TEXT UNIQUE NOT NULL, "
        "user_login TEXT UNIQUE NOT NULL, "
        "about TEXT DEFAULT NULL, "
        "public_key TEXT NOT NULL, "
        "private_key TEXT NOT NULL, "
        "created_at TIMESTAMP DEFAULT (strftime('%s', 'now'))"
        ");";
    vendor.database.sql.execute_sql(sql);
}

static void add_user(t_active_users_struct *user) {
    char sql[10240];
    snprintf(sql, sizeof(sql),
             "INSERT INTO active_users "
             "(user_id, username, user_login, public_key, private_key) "
             "VALUES (%d, '%s', '%s', '%s', '%s');",
             user->user_id, user->username, user->user_login, user->public_key, user->private_key);

    vendor.database.sql.execute_sql(sql);
}

static t_active_users_struct *get_user_by_user_login(const char *user_login) {
    char sql[512];
    snprintf(sql, sizeof(sql),
             "SELECT user_id, username, user_login, about, public_key, private_key FROM active_users WHERE user_login = '%s';",
             user_login);

    char **results = NULL;
    int rows, cols;
    int rc = vendor.database.sql.execute_query(sql, &results, &rows, &cols);

    if (rc != 0 || rows == 0) {
        printf("No results found for user_login: %s\n", user_login);
        return NULL;
    }

    t_active_users_struct *user = (t_active_users_struct *)malloc(sizeof(t_active_users_struct));
    if (user == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    user->user_id = atoi(results[cols]);
    user->username = vendor.helpers.strdup(results[cols + 1]);
    user->user_login = vendor.helpers.strdup(results[cols + 2]);
    user->about = results[cols + 3] ? vendor.helpers.strdup(results[cols + 3]) : NULL;
    user->public_key = vendor.helpers.strdup(results[cols + 4]);
    user->private_key = vendor.helpers.strdup(results[cols + 5]);

//    vendor.database.sql.free(results);

    return user;
}

static void free_struct(t_active_users_struct *user) {
    if (user != NULL) {
        if (user->username != NULL) {
            free(user->username);
        }
        if (user->user_login != NULL) {
            free(user->user_login);
        }
        if (user->about != NULL) {
            free(user->about);
        }
        if (user->public_key != NULL) {
            free(user->public_key);
        }
        if (user->private_key != NULL) {
            free(user->private_key);
        }
        free(user);
    }
}

t_active_users_table init_active_users_table(void) {
    t_active_users_table table = {
        .create_table = create_active_users_table,
        .add_user = add_user,
        .get_user_by_user_login = get_user_by_user_login,
        .free_struct = free_struct,
    };

    return table;
}
