#include "settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "utils/macros.h"

#define BUF_SIZE 1024

char *file_to_string(char *path)
{
    char buf[BUF_SIZE];

    FILE *user_file = fopen(path, "r");
    if (user_file == NULL)
    {
        log_error("Failed to open file '%s'.", path);
        return NULL;
    }
    size_t count = fread(buf, sizeof(char), BUF_SIZE, user_file);

    char *s = malloc((count + 1) * sizeof(char));
    if (s == NULL)
    {
        log_alloc_error();
        return NULL;
    }

    s[count] = '\0';
    memcpy(s, buf, count * sizeof(char));

    return s;
}

struct settings *settings_create(int argc, char *argv[])
{
    struct settings *settings = malloc(sizeof(struct settings));
    if (settings == NULL)
    {
        log_alloc_error();
        return NULL;
    }

    char *host = file_to_string("host");
    char *port = file_to_string("port");

    if (port == NULL || host == NULL)
    {
        free(settings);
        free(port);
        free(host);
        return NULL;
    }

    struct sock *sock = sock_create(host, port);
    free(port);
    free(host);
    char *user_hash = file_to_string("user_hash");
    char *machine_hash = file_to_string("machine_hash");
    char *version = file_to_string("version");
    if (sock == NULL || user_hash == NULL || machine_hash == NULL
        || version == NULL)
    {
        sock_destroy(sock);
        free(user_hash);
        free(machine_hash);
        free(version);
        free(settings);
        return NULL;
    }

    settings->sock = sock;
    settings->user_hash = user_hash;
    settings->machine_hash = machine_hash;
    settings->version = version;

    for (int i = 1; i < argc; i++)
    {
        if (STREQL(argv[i], "-v"))
        {
            settings->verbose = true;
            log_verbose(true, "Running in verbose mode.");
        }
    }

    return settings;
}

void settings_destroy(struct settings *settings)
{
    if (settings == NULL)
    {
        return;
    }

    sock_destroy(settings->sock);
    free(settings->user_hash);
    free(settings->machine_hash);
    free(settings->version);
    free(settings);
}
