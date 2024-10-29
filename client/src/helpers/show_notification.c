#include "../../inc/header.h"
#include <gtk/gtk.h>

void ubuntu_show_notification(const char *title, const char *message) {
    NotifyNotification *notification;

    notification = notify_notification_new(title, message, NULL);

    GError *error = NULL;
    if (!notify_notification_show(notification, &error)) {
        g_error("Failed to send notification: %s", error->message);
        g_error_free(error);
    }

    g_object_unref(G_OBJECT(notification));
}

void macos_show_notification(const char *title, const char *message) {
    char command[512];
    snprintf(command, sizeof(command), "osascript -e 'display notification \"%s\" with title \"%s\"'", message, title);
    system(command);
}
