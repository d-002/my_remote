#include "mainloop.h"

#include <stdlib.h>
#include <unistd.h>

#include "logger/logger.h"
#include "sock/sock.h"
#include "sock/sockutils.h"
#include "utils/errors.h"
#include "utils/stringbuilder.h"

static int apply_patch(struct settings *settings, struct string patch)
{
    log_info("New version detected, applying...");
    patch.data++;

    log_info("Restarting in a new process...");
    int pid = fork();

    if (pid < 0)
    {
        log_info("Failed to start new process.");
        return ERROR;
    }
    if (pid == 0)
    {
        char *argv[] = {
            settings->argv0,
            NULL,
        };

        execvp(argv[0], argv);

        log_info("Should not happen.");
        return ERROR;
    }
    else
    {
        return EXIT;
    }
}

static int heartbeat(struct settings *settings, struct state *state)
{
    log_verbose(settings->verbose, "Heartbeat.");

    struct string url = NULL_STRING;
    struct string content = NULL_STRING;
    struct string response = NULL_STRING;
    int which_err = ERROR;

    char *url_arr[] = {
        "/api/heartbeat.php?user=",
        settings->user_hash,
        "&machine=",
        settings->machine_hash,
        NULL,
    };
    url = concat_str(url_arr);
    if (url.data == NULL)
    {
        which_err = FATAL;
        goto error;
    }

    char *content_arr[] = {
        state_str(state),
        "\n",
        settings->version,
        NULL,
    };
    content = concat_str(content_arr);
    if (content.data == NULL)
    {
        which_err = FATAL;
        goto error;
    }

    struct sock *sock = sock_request(settings, "POST", url.data, content);
    if (sock == NULL)
    {
        goto error;
    }

    response = recv_content(sock);
    if (response.data == NULL)
    {
        goto error;
    }

    if (STRSTARTSWITH(response.data, "error"))
    {
        log_error("%s", response.data);
        goto error;
    }
    else if (STRSTARTSWITH(response.data, "update"))
    {
        size_t offset = strlen("update\n");
        struct string patch = {
            .data = response.data + offset,
            .length = response.length - offset,
        };

        int res = apply_patch(settings, patch);
        if (res != SUCCESS)
        {
            which_err = res;
            goto error;
        }
    }

    sock_destroy(sock);
    STRING_FREE(url);
    STRING_FREE(content);
    STRING_FREE(response);

    return SUCCESS;

error:
    sock_destroy(sock);
    STRING_FREE(url);
    STRING_FREE(content);
    STRING_FREE(response);

    return which_err;
}

int mainloop(struct settings *settings, struct state *state)
{
    while (true)
    {
        int res = heartbeat(settings, state);
        if (res == FATAL || res == EXIT)
        {
            return res;
        }

        state_sleep(state);
    }

    return SUCCESS;
}
