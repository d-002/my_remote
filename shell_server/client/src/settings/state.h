#ifndef STATE_H
#define STATE_H

#define SLEEP_IDLE 10
#define SLEEP_ACTIVE 1
#define ACTIVE_TIME 15

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

char *state_str(struct state *state);
void state_sleep(struct state *state);

#endif /* ! STATE_H */
