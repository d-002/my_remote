#include <stddef.h>

#include "logger/logger.h"
#include "mainloop/mainloop.h"
#include "mainloop/signals.h"
#include "settings/settings.h"
#include "settings/state.h"
#include "utils/errors.h"

int main(int argc, char *argv[])
{
    int res = SUCCESS;

    struct settings *settings = settings_create(argc, argv);
    struct state *state = state_create();
    if (settings == NULL || state == NULL)
    {
        res = FATAL;
        goto end;
    }

    res = setup_signals(settings);
    if (res != SUCCESS)
    {
        goto end;
    }

    res = mainloop(settings, state);

    if (res == EXIT)
    {
        log_info("Graceful exit.");
        res = SUCCESS;
    }

end:
    settings_destroy(settings);
    state_destroy(state);
    return res;
}
