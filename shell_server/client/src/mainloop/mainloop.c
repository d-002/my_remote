#include "mainloop.h"

#include <stdlib.h>

#include "logger/logger.h"
#include "utils/macros.h"
#include "utils/sockutils.h"

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
    if (url.data == NULL) {
        return ERROR;
    }

    char *content_arr[] = {
        state_str(state),
        "\n",
        settings->version,
        NULL,
    };
    struct string content = concat_str(content_arr);
    if (content.data == NULL) {
        free(content.data);
        return ERROR;
    }

    ssize_t count = sock_request(settings->sock, "POST", url.data, content);
    log_info("%ld", count);

    return SUCCESS;
}

int mainloop(struct settings *settings, struct state *state)
{
    while (1)
    {
        int res = heartbeat(settings, state);
        if (res != SUCCESS) {
            return res;
        }

        state_sleep(state);
    }

    return SUCCESS;
}
