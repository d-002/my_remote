#ifndef STATE_H
#define STATE_H

enum state_values {
    IDLE,
    ACTIVE,
};

struct state {
    enum state_values state;

    // last time an event was received or sent
    int last_action_timestamp;
};

struct state *state_create(void);
void state_destroy(struct state *state);

#endif /* ! STATE_H */
