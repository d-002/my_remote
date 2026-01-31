#include "mainloop.h"
#include "utils/macros.h"

#include "utils/sockutils.h"
#include "logger/logger.h"

static int heartbeat(struct settings *settings) {
    log_verbose(settings->verbose, "Heartbeat.");

    return SUCCESS;
}

int mainloop(struct settings *settings, struct state *state) {
    while (1) {
        heartbeat(settings);
        state_sleep(state);
    }

    return SUCCESS;
}
