#include "cmd_list.h"

#include <stdlib.h>

#include "command.h"
#include "logger/logger.h"
#include "sock/sock.h"
#include "sock/sockutils.h"
#include "utils/errors.h"
#include "utils/stringbuilder.h"

static int list_commands_prepare(struct settings *settings, struct sock **out)
{
    struct string url = NULL_STRING;
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
        goto end;
    }

    struct sock *sock = sock_request(settings, "GET", url.data, NULL_STRING);
    if (sock == NULL)
    {
        err = ERROR;
        goto end;
    }

    *out = sock;

end:
    STRING_FREE(url);

    return err;
}

int split_add_commands(struct string data, struct queue *queue)
{
    while (1)
    {
        char *ptr = strchr(data.data, '\n');
        size_t length = ptr == NULL ? data.length : (size_t)(ptr - data.data);
        if (length == 0)
        {
            break;
        }

        struct command *command = command_create(data.data, length);
        if (command == NULL)
        {
            log_alloc_error("creating command from response");
            return FATAL;
        }

        if (queue_enqueue(queue, command))
        {
            log_alloc_error("enqueueing command");
            command_destroy(command);
            return FATAL;
        }

        data.data += length + 1;
        data.length -= length + 1;
    }

    return SUCCESS;
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
        err = ERROR;
        goto end;
    }
    else
    {
        // found commands, now list and categorize them
        // will assume they are well formed
        err = split_add_commands(response, queue);
    }

end:
    sock_destroy(sock);
    STRING_FREE(response);
    return err;
}
