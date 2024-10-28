#ifndef INPUT_H
#define INPUT_H

#include "components.h"

typedef struct t_input {
    GtkWidget *(*create)(const gchar *name, const gchar *placeholder, GCallback input_handler);
} t_input;

GtkWidget *create_input(const gchar *name, const gchar *placeholder, GCallback input_handler);

#endif //INPUT_H
