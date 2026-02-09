#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

#include "settings/settings.h"

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

char *state_str(enum state_values state);

/*
 * Set the state depending on whether an action was taken or nothing was done
 * for too long, then sleep accordingly.
 * Return whether the state changed this call.
 */
bool state_sleep(struct settings *settings, struct state *state, bool action);

#endif /* ! STATE_H */
