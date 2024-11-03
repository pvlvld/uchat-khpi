#ifndef TITLE_H
#define TITLE_H

#include "components.h"

typedef struct {
    GtkWidget *(*create_h1)(const char *title);
} t_title;

GtkWidget *create_title_h1(const char *title);

#endif //TITLE_H
