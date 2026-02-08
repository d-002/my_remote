#include "run.h"

#include "command.h"
#include "logger/logger.h"
#include "utils/errors.h"

static int run_command(struct settings *settings, struct command *command)
{
    if (command->is_read || command->sender != USER)
        return SUCCESS;

    log_verbose(settings->verbose, "Running command '%s'", command->content);

    return SUCCESS;
}

int run_all_commands(struct settings *settings, struct queue *queue)
{
    for (struct command *command = queue_dequeue(queue); command;
         command = queue_dequeue(queue))
    {
        int err = run_command(settings, command);
        command_destroy(command);

        if (err == FATAL || err == EXIT)
        {
            return err;
        }
    }

    return SUCCESS;
}
