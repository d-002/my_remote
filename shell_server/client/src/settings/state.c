#include "state.h"

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "logger/logger.h"

#define SLEEP_IDLE 10
#define SLEEP_ACTIVE 1
#define IDLE_THRESHOLD 15

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

void state_sleep(struct settings *settings, struct state *state, bool action)
{
    int now = time(NULL);

    if (action)
    {
        state->last_action_timestamp = now;
    }

    enum state_values prev = state->state;
    state->state =
        now - state->last_action_timestamp < IDLE_THRESHOLD ? ACTIVE : IDLE;

    if (prev != state->state)
    {
        log_verbose(settings->verbose, "Switching state to %s",
                    state_str(state->state));
    }

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

char *state_str(enum state_values state)
{
    static char idle_str[] = "idle";
    static char active_str[] = "active";

    switch (state)
    {
    case IDLE:
        return idle_str;
    case ACTIVE:
        return active_str;
    }

    return NULL;
}
