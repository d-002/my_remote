#include "mainloop.h"

#include <stdlib.h>

#include "logger/logger.h"
#include "utils/macros.h"
#include "utils/sockutils.h"

static int apply_patch(struct settings *settings, struct string patch) {
    settings++; /////
    patch.data++; /////

    return SUCCESS;
}

static int heartbeat(struct settings *settings, struct state *state)
{
    log_verbose(settings->verbose, "Heartbeat.");

    char *url_arr[] = {
        "/api/heartbeat.php?user=",
        settings->user_hash,
        "&machine=",
        settings->machine_hash,
        NULL,
    };
    struct string url = concat_str(url_arr);
    if (url.data == NULL)
    {
        return ERROR;
    }

    char *content_arr[] = {
        state_str(state),
        "\n",
        settings->version,
        NULL,
    };
    struct string content = concat_str(content_arr);
    if (content.data == NULL)
    {
        STRING_FREE(content);
        return ERROR;
    }

    ssize_t count = sock_request(settings->sock, "POST", url.data, content);
    if (count < 0)
    {
        return ERROR;
    }

    struct string response = recv_content(settings->sock);
    if (response.data == NULL)
    {
        return ERROR;
    }

    if (STRSTARTSWITH(response.data, "error"))
    {
        log_error("%s", response.data);
    }
    else if (STRSTARTSWITH(response.data, "update"))
    {
        size_t offset = strlen("update\n");
        struct string patch = {
            .data = response.data + offset,
            .length = response.length - offset,
        };

        apply_patch(settings, patch);
    }

    STRING_FREE(response);

    return SUCCESS;
}

int mainloop(struct settings *settings, struct state *state)
{
    while (1)
    {
        int res = heartbeat(settings, state);
        if (res != SUCCESS)
        {
            return res;
        }

        state_sleep(state);
    }

    return SUCCESS;
}
