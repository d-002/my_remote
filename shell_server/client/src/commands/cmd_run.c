#include "cmd_run.h"

#include <stdlib.h>

#include "comm/comm_api.h"
#include "command.h"
#include "logger/logger.h"
#include "utils/errors.h"

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
        log_info("got back '%s'", out.data);
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
