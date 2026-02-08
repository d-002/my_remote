#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>

struct settings {
    char *argv0;

    char *host;
    char *port;
    char *user_hash;
    char *machine_hash;
    char *version;

    bool verbose;

    int shell_fd;
};

struct settings *settings_create(int argc, char *argv[]);
void settings_destroy(struct settings *settings);

#endif /* ! SETTINGS_H */
