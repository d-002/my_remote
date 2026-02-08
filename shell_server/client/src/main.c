#include <stddef.h>

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>

#include "errno.h"
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

    int safety = 0;
    while (safety++ < 10)
    {
        size_t len = 0;
        char *buf = NULL;

        ssize_t count = getline(&buf, &len, stdin);
        if (count < 0)
        {
            printf("getline error: %s\n", strerror(errno));
            break;
        }
        if (count == 0) {
            printf("no input in getline\n");
            break;
        }

        fwrite(buf, 1, count, stdout);
        count = write(settings->shell_fd[1], buf, count);
        if (count < 0)
        {
            printf("write error: %s\n", strerror(errno));
            break;
        }
        write(settings->shell_fd[1], "\0", 1);

        printf("done with command, start feedback\n");
        while (1)
        {
            count = read(settings->shell_fd[0], buf, len);
            if (count < 0)
            {
                printf("read error: %s\n", strerror(errno));
                break;
            }
            if (count == 0)
                break;

            fwrite(buf, 1, count, stdout);
        }
        printf("done with feedback\n");
    }
    goto end; //////////

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
