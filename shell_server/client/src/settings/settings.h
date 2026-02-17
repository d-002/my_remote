#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>

#define HOST_FILE "host"
#define PORT_FILE "port"
#define USER_HASH_FILE "user_hash"
#define MACHINE_HASH_FILE "machine_hash"
#define VERSION_FILE "version"

struct settings {
    char **argv;

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
