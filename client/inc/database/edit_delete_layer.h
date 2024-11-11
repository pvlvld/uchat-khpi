#ifndef EDIT_DELETE_LAYER_H
#define EDIT_DELETE_LAYER_H

typedef struct {
    int (*delete_message)(int message_id);
    int (*edit_message)(int message_id, const char *new_message_text);
} t_edit_delete_layer;

t_edit_delete_layer init_edit_delete_layer(void);

#endif // EDIT_DELETE_LAYER_H
