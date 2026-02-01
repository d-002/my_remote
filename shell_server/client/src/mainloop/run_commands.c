#include "run_commands.h"

#include <stdlib.h>

#include "logger/logger.h"
#include "utils/errors.h"

static int run_command(struct settings *settings, char *command)
{
    log_verbose(settings->verbose, "running command '%s'", command);

    return SUCCESS;
}

int run_all_commands(struct settings *settings, struct queue *queue)
{
    for (char *command = queue_dequeue(queue); command;
         command = queue_dequeue(queue))
    {
        int err = run_command(settings, command);
        free(command);

        if (err == FATAL || err == EXIT)
        {
            return err;
        }
    }

    return SUCCESS;
}
