#include "../inc/env.h"
#include "sys/socket.h"
t_env init_env(void) {
    const t_env env = {
        .port = 8080,
		.max_clients = SOMAXCONN,
		.dev_mode = 1,
        .db_name = "name.db",
        .jwt_secret_key = "your_secret_key",
        .sms_dev_mode = 1,
        .sms_send_mode = 0,
        .code_length = 6,
        .account_sid = "your_account_sid",
        .auth_token = "your_auth_token",
        .sender_phone_number = "sender_sender_phone",
        .recipient_phone_number = "recipient_sender_phone",
    };
    return env;
}
