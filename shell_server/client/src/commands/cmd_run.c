#include "cmd_run.h"

#include <stdlib.h>
#include <stdio.h>

#include "sock/sockutils.h"
#include "utils/stringbuilder.h"
#include "comm/comm_api.h"
#include "command.h"
#include "logger/logger.h"
#include "utils/errors.h"

static int mark_command_as_read(struct settings *settings) {
    struct string url = NULL_STRING;
    int err = SUCCESS;

    char *url_arr[] = {
        "/api/machine_read_command.php?user=",
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

    struct string response = recv_content(sock);
    if (response.data == NULL) {
        err = ERROR;
        goto end;
    }

    if (STRSTARTSWITH(response.data, "error"))
    {
        log_error("%s", response.data);
        err = ERROR;
        goto end;
    }

end:
    STRING_FREE(url);

    return err;
}

static int run_command(struct settings *settings, struct command *command,
                       bool *action)
{
    if (command->is_read || command->sender != USER)
        return SUCCESS;

    log_verbose(settings->verbose, "Running command '%s'", command->content);
    *action = true;

    int err = send_sh(settings, command->content);
    if (err != SUCCESS)
    {
        return err;
    }

    struct string out = NULL_STRING;
    err = recv_sh(settings, &out);

    if (err == SUCCESS)
    {
        printf("%s\n", out.data);
        err = mark_command_as_read(settings);
        STRING_FREE(out);
    }

    return err;
}

int run_all_commands(struct settings *settings, struct queue *queue,
                     bool *action)
{
    for (struct command *command = queue_dequeue(queue); command;
         command = queue_dequeue(queue))
    {
        int err = run_command(settings, command, action);
        command_destroy(command);

        if (err == FATAL || err == EXIT)
        {
            return err;
        }
    }

    return SUCCESS;
}
