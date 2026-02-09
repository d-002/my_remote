#include "heartbeat.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "commands/cmd_special.h"
#include "logger/logger.h"
#include "sock/sockutils.h"
#include "utils/errors.h"
#include "utils/stringbuilder.h"
#include "utils/stringutils.h"

static int write_to_file(struct settings *settings, char *fname, char *data,
                         size_t len)
{
    FILE *f = fopen(fname, "w");
    if (f == NULL)
    {
        log_error("Failed to open '%s' for writing.", fname);
        return ERROR;
    }

    int err = SUCCESS;
    while (len > 0)
    {
        ssize_t count = fwrite(data, sizeof(char), len, f);
        if (count < 0)
        {
            err = ERROR;
            break;
        }

        len -= count;
        data += count;

        log_verbose(settings->verbose, "Wrote %ld bytes, %ld remaining", count,
                    len);
    }

    fclose(f);
    return err;
}

static int apply_patch(struct settings *settings, struct string patch)
{
    char *ptr = strchr(patch.data, '\n');
    size_t version_len = ptr - patch.data;

    char *new_version = patch.data;
    new_version[version_len] = '\0';

    log_info("New version detected (%s, current is %s).", new_version,
             settings->version);

    log_info("Writing new version to file...");
    int err = write_to_file(settings, VERSION_FILE, new_version, version_len);
    if (err != SUCCESS)
    {
        return err;
    }

    patch.data += version_len + 1;
    patch.length -= version_len + 1;

    log_info("Writing new software to file...");
    size_t filename_len = strlen(settings->argv[0]);
    char *temp_name = calloc(filename_len + 2, sizeof(char));
    if (temp_name == NULL)
    {
        log_alloc_error("temp file name allocation");
        return FATAL;
    }
    memcpy(temp_name, settings->argv[0], filename_len);
    temp_name[filename_len] = '~';

    err = write_to_file(settings, temp_name, patch.data, patch.length);
    if (err != SUCCESS)
    {
        free(temp_name);
        return err;
    }

    log_info("Renaming temporary file to current file...");
    chmod(temp_name, 0755);
    rename(temp_name, settings->argv[0]);

    free(temp_name);
    return err;
}

static int update(struct settings *settings, struct string patch)
{
    int err = apply_patch(settings, patch);
    if (err != SUCCESS)
    {
        return err;
    }

    return restart(settings);
}

int heartbeat(struct settings *settings, struct state *state)
{
    log_verbose(settings->verbose, "Heartbeat.");

    char *url_arr[] = {
        "/api/heartbeat.php?user=",
        settings->user_hash,
        "&machine=",
        settings->machine_hash,
        "&state=",
        state_str(state->state),
        NULL,
    };
    char *content_arr[] = {
        state_str(state->state),
        "\n",
        settings->version,
        NULL,
    };
    struct string content = concat_str(content_arr);
    if (content.data == NULL)
    {
        return FATAL;
    }

    struct string response = NULL_STRING;
    int err = post_wrapper(settings, url_arr, content, &response);
    STRING_FREE(content);
    if (err != SUCCESS)
    {
        goto end;
    }

    if (STRSTARTSWITH(response.data, "update"))
    {
        size_t offset = strlen("update\n");
        struct string patch = {
            .data = response.data + offset,
            .length = response.length - offset,
        };

        err = update(settings, patch);
        if (err != SUCCESS)
        {
            goto end;
        }
    }

end:
    STRING_FREE(response);

    return err;
}
