#ifndef CHAT_H
#define CHAT_H

#include "header.h"

typedef struct {
    GtkWidget *(*init)(void);
} t_chat;

t_chat init_chat(void);

#endif //CHAT_H
