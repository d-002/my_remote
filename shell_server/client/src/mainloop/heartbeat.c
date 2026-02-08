#include "heartbeat.h"

#include <stdlib.h>
#include <unistd.h>

#include "logger/logger.h"
#include "sock/sock.h"
#include "sock/sockutils.h"
#include "utils/errors.h"
#include "utils/stringbuilder.h"
#include "utils/stringutils.h"

static int apply_patch(struct settings *settings, struct string patch)
{
    char *ptr = strchr(patch.data, '\n');
    size_t len = ptr - patch.data;

    char *new_version = patch.data;
    new_version[len] = '\0';
    patch.data += len + 1;

    log_info("New version detected (%s, current is %s)", new_version, settings->version);
    return SUCCESS;

    log_info("Writing patch to file...");
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

static int heartbeat_prepare(struct settings *settings, struct state *state,
                             struct sock **out)
{
    struct string url = NULL_STRING;
    struct string content = NULL_STRING;
    int err = SUCCESS;

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
        err = FATAL;
        goto end;
    }

    char *content_arr[] = {
        state_str(state->state),
        "\n",
        settings->version,
        NULL,
    };
    content = concat_str(content_arr);
    if (content.data == NULL)
    {
        err = FATAL;
        goto end;
    }

    struct sock *sock = sock_request(settings, "POST", url.data, content);
    if (sock == NULL)
    {
        err = ERROR;
        goto end;
    }

    *out = sock;

end:
    STRING_FREE(url);
    STRING_FREE(content);

    return err;
}

int heartbeat(struct settings *settings, struct state *state)
{
    log_verbose(settings->verbose, "Heartbeat.");

    struct sock *sock = NULL;
    int err = heartbeat_prepare(settings, state, &sock);

    if (err != SUCCESS)
    {
        goto end;
    }

    struct string response = recv_content(sock);
    if (response.data == NULL)
    {
        err = ERROR;
        goto end;
    }

    if (STRSTARTSWITH(response.data, "error"))
    {
        log_error("%s", response.data);
        goto end;
    }
    else if (STRSTARTSWITH(response.data, "update"))
    {
        size_t offset = strlen("update\n");
        struct string patch = {
            .data = response.data + offset,
            .length = response.length - offset,
        };

        err = apply_patch(settings, patch);
        if (err != SUCCESS)
        {
            goto end;
        }
    }

end:
    sock_destroy(sock);
    STRING_FREE(response);

    return err;
}
