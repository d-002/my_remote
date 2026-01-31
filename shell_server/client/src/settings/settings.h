#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>

#include "utils/sockutils.h"

struct settings {
    struct sock *sock;

    char *user_hash;
    char *machine_hash;
    char *version;

    bool verbose;
};

struct settings *settings_create(int argc, char *argv[]);
void settings_destroy(struct settings *settings);

#endif /* ! SETTINGS_H */
