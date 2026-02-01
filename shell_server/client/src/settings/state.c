#include "state.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "logger/logger.h"

struct state *state_create(void)
{
    struct state *state = malloc(sizeof(struct state));
    if (state == NULL)
    {
        log_alloc_error("state create");
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

void state_sleep(struct state *state, bool action)
{
    int now = time(NULL);

    if (action)
    {
        state->last_action_timestamp = now;
    }

    state->state =
        now - state->last_action_timestamp < IDLE_THRESHOLD ? ACTIVE : IDLE;

    switch (state->state)
    {
    case IDLE:
        sleep(SLEEP_IDLE);
        break;
    case ACTIVE:
        sleep(SLEEP_ACTIVE);
        break;
    }
}

char *state_str(struct state *state)
{
    static char idle_str[] = "idle";
    static char active_str[] = "idle";

    switch (state->state)
    {
    case IDLE:
        return idle_str;
    case ACTIVE:
        return active_str;
    }

    return NULL;
}
