#include "mainloop/mainloop.h"
#include "settings/settings.h"
#include "settings/state.h"
#include "utils/macros.h"

int main(int argc, char *argv[])
{
    struct settings *settings = settings_create(argc, argv);
    struct state *state = state_create();
    if (settings == NULL || state == NULL)
    {
        settings_destroy(settings);
        state_destroy(state);
        return ERROR;
    }

    int res = mainloop(settings, state);

    settings_destroy(settings);
    state_destroy(state);
    return res;
}
