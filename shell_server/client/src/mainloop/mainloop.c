#include "mainloop.h"

#include <stdlib.h>

#include "heartbeat.h"
#include "list_commands.h"
#include "queue/queue.h"
#include "run_commands.h"
#include "utils/errors.h"

int mainloop(struct settings *settings, struct state *state)
{
    struct queue *queue = queue_create(free);
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
