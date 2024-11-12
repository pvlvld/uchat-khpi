#include "../../inc/header.h"
#include <gtk/gtk.h>

void *play_sound_thread(void *arg) {
    const char *command = (const char *)arg;
    system(command);
    return NULL;
}

static void play_sound_ubuntu(void) {
    pthread_t thread;
    const char *command = "aplay ./resources/sounds/naruto-shadow-clone-jutsu-free-sound-effect.wav > /dev/null 2>&1";

    if (pthread_create(&thread, NULL, play_sound_thread, (void *)command) != 0) {
        return;
    }

    pthread_detach(thread);
}

static void play_sound_macos(void) {
    pthread_t thread;
    const char *command = "afplay ./resources/sounds/naruto-shadow-clone-jutsu-free-sound-effect.wav > /dev/null 2>&1";

    if (pthread_create(&thread, NULL, play_sound_thread, (void *)command) != 0) {
        return;
    }

    pthread_detach(thread);
}

void ubuntu_show_notification(const char *title, const char *message) {
    play_sound_ubuntu();

    char command[512];

    snprintf(command, sizeof(command), "notify-send -a 'ShadowTalk' '%s' '%s' >/dev/null 2>&1", title, message);

    system(command);
}

void macos_show_notification(const char *title, const char *message) {
    play_sound_macos();

    char command[512];

    snprintf(command, sizeof(command), "osascript -e 'display notification \"%s\" with title \"%s\"'", message, title);

    system(command);
}

void show_notification(const char *title, const char *message) { // Definition only in this file
#ifdef __APPLE__
    macos_show_notification(title, message);
#else
    ubuntu_show_notification(title, message);
#endif
}
