#include "mainloop.h"

#include "heartbeat.h"
#include "commands/command.h"
#include "commands/list.h"
#include "commands/run.h"
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

        err = run_all_commands(settings, queue);
        if (err == FATAL || err == EXIT)
        {
            goto end;
        }

        bool action = queue->length != 0;
        state_sleep(settings, state, action);
    }

end:
    queue_destroy(queue);
    return err;
}
