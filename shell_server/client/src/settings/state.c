#include "state.h"

#include <stdlib.h>

#include "logger/logger.h"

struct state *state_create(void)
{
    struct state *state = malloc(sizeof(struct state));
    if (state == NULL)
    {
        log_alloc_error();
        return NULL;
    }

    state->state = IDLE;
    state->last_action_timestamp = 0;

    return state;
}

void state_destroy(struct state *state)
{
    if (state == NULL)
    {
        return;
    }
    free(state);
}
