#include <stddef.h>

#include <unistd.h>

#include "logger/logger.h"
#include "mainloop/mainloop.h"
#include "mainloop/signals.h"
#include "settings/settings.h"
#include "settings/state.h"
#include "string.h"
#include "utils/errors.h"

int main(int argc, char *argv[])
{
    int err = SUCCESS;

    struct settings *settings = settings_create(argc, argv);
    struct state *state = state_create();
    if (settings == NULL || state == NULL)
    {
        err = FATAL;
        goto end;
    }

    err = setup_signals(settings);
    if (err != SUCCESS)
    {
        goto end;
    }

    err = mainloop(settings, state);

    if (err == EXIT)
    {
        log_info("Graceful exit.");
        err = SUCCESS;
    }

end:
    settings_destroy(settings);
    state_destroy(state);
    return err;
}
