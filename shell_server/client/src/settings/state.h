#ifndef STATE_H
#define STATE_H

#define SLEEP_IDLE 10
#define SLEEP_ACTIVE 1
#define IDLE_THRESHOLD 15

#include <stdbool.h>

enum state_values
{
    IDLE,
    ACTIVE,
};

struct state
{
    enum state_values state;

    // last time an event was received or sent
    int last_action_timestamp;
};

struct state *state_create(void);
void state_destroy(struct state *state);

char *state_str(struct state *state);

// set the state depending on whether an action was taken or nothing was done
// for too long, then sleep accordingly
void state_sleep(struct state *state, bool action);

#endif /* ! STATE_H */
