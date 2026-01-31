#include "mainloop.h"

#include <stdlib.h>

#include "logger/logger.h"
#include "sock/sockutils.h"
#include "utils/errors.h"
#include "utils/stringbuilder.h"

static int apply_patch(struct settings *settings, struct string patch)
{
    settings++; /////
    patch.data++; /////

    return SUCCESS;
}

static int heartbeat(struct settings *settings, struct state *state)
{
    log_verbose(settings->verbose, "Heartbeat.");

    struct string url = NULL_STRING;
    struct string content = NULL_STRING;
    struct string response = NULL_STRING;
    int if_err = ERROR;

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
        if_err = FATAL;
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
        if_err = FATAL;
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

    log_info("'%s'", response.data);
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
            if_err = res;
            goto error;
        }
    }

    STRING_FREE(url);
    STRING_FREE(content);
    STRING_FREE(response);

    return SUCCESS;

error:
    STRING_FREE(url);
    STRING_FREE(content);
    STRING_FREE(response);

    return if_err;
}

int mainloop(struct settings *settings, struct state *state)
{
    while (true)
    {
        int res = heartbeat(settings, state);
        if (res == FATAL)
        {
            return res;
        }

        state_sleep(state);
    }

    return SUCCESS;
}
