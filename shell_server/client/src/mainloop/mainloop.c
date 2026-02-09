#include "mainloop.h"

#include "commands/cmd_list.h"
#include "commands/cmd_run.h"
#include "commands/command.h"
#include "heartbeat.h"
#include "queue/queue.h"
#include "utils/errors.h"

int mainloop(struct settings *settings, struct state *state)
{
    struct queue *queue = queue_create(command_destroy);
    if (queue == NULL)
    {
        return FATAL;
    }

    int err = SUCCESS;

    while (true)
    {
        err = heartbeat(settings, state);
        if (err == FATAL || err == EXIT)
        {
            goto end;
        }

        err = list_commands(settings, queue);
        if (err == FATAL)
        {
            goto end;
        }

        bool action = false;
        err = run_all_commands(settings, queue, &action);
        if (err == FATAL || err == EXIT)
        {
            goto end;
        }

        if (state_sleep(settings, state, action) && err == SUCCESS)
        {
            // if the state changed, notify the server
            err = heartbeat(settings, state);
        }
    }

end:
    queue_destroy(queue);
    return err;
}
