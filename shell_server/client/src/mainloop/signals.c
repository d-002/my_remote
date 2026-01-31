#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200112L
#endif /* ! _POSIX_C_SOURCE */

#include "signals.h"

#include <sys/signal.h>

#include "logger/logger.h"
#include "utils/errors.h"

struct myglobals
{
    struct settings *settings;
};

static struct myglobals myglobals = {
    .settings = NULL,
};

static void handler(int signum)
{
    switch (signum)
    {
    case SIGPIPE:
        log_verbose(myglobals.settings->verbose, "Caught SIGPIPE, ignoring.");
        return;
    }
}

int setup_signals(struct settings *settings)
{
    myglobals.settings = settings;

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = handler;

    if (sigemptyset(&sa.sa_mask) < 0)
        return FATAL;
    if (sigaction(SIGPIPE, &sa, NULL) < 0)
        return FATAL;
    if (sigaction(SIGINT, &sa, NULL) < 0)
        return FATAL;

    return SUCCESS;
}
