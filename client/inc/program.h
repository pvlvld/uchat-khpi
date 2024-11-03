#ifndef PROGRAM_H
#define PROGRAM_H

#include "header.h"

GtkWidget *init_window(GtkApplication *app);
GdkScreen *init_screen(void);
GtkCssProvider *init_css_provider(const char *path);

#endif //PROGRAM_H
