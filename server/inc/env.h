#ifndef ENV_H
#define ENV_H

#include <stdlib.h>

typedef struct {
    unsigned short port;
    unsigned int max_clients;
    int dev_mode;
    char* db_name;
    const char *jwt_secret_key;
	int sms_dev_mode;
	int sms_send_mode;
    unsigned short code_length;
    const char *account_sid;
    const char *auth_token;
    const char *sender_phone_number;
    const char *recipient_phone_number;
} t_env;

t_env init_env(void);

#endif //ENV_H
