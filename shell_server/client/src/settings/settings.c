#include "settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "comm/comm_setup.h"
#include "logger/logger.h"
#include "utils/stringutils.h"

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
        log_alloc_error("file to string");
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
        log_alloc_error("settings create");
        return NULL;
    }

    for (int i = 1; i < argc; i++)
    {
        if (STREQL(argv[i], "-v"))
        {
            settings->verbose = true;
        }
    }
    log_verbose(settings->verbose, "Running in verbose mode.");

    char *host = file_to_string("host");
    char *port = file_to_string("port");
    char *user_hash = file_to_string("user_hash");
    char *machine_hash = file_to_string("machine_hash");
    char *version = file_to_string("version");

    int shell_fd = comm_setup(settings);

    if (host == NULL || port == NULL || user_hash == NULL
        || machine_hash == NULL || version == NULL || shell_fd < 0)
    {
        free(host);
        free(port);
        free(user_hash);
        free(machine_hash);
        free(version);
        free(settings);
        close(shell_fd);
        return NULL;
    }

    settings->argv0 = argv[0];
    settings->port = port;
    settings->host = host;
    settings->user_hash = user_hash;
    settings->machine_hash = machine_hash;
    settings->version = version;
    settings->shell_fd = shell_fd;

    log_verbose(settings->verbose, "Successfully read config.");
    log_verbose(settings->verbose, "Machine hash - %s", settings->machine_hash);
    log_verbose(settings->verbose, "User hash ---- %s", settings->user_hash);
    log_verbose(settings->verbose, "Soft version - %s", settings->version);

    return settings;
}

void settings_destroy(struct settings *settings)
{
    if (settings == NULL)
    {
        return;
    }

    free(settings->host);
    free(settings->port);
    free(settings->user_hash);
    free(settings->machine_hash);
    free(settings->version);
    close(settings->shell_fd);
    free(settings);
}
