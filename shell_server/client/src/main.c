#include <stddef.h>

#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */
#include <stdio.h>
#include <unistd.h>

#include "comm/comm.h"
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

        printf("> ");
        ssize_t count = getline(&buf, &len, stdin);
        if (count < 0)
        {
            printf("getline error: %s\n", strerror(errno));
            break;
        }
        if (count == 0)
        {
            printf("no input in getline\n");
            break;
        }

        count = write(settings->shell_fd, buf, count);
        char padding[] = "echo '<END_TAG>'\n";
        count = write(settings->shell_fd, padding, sizeof(padding));
        if (count < 0)
        {
            printf("write error: %s\n", strerror(errno));
            break;
        }

        while (1)
        {
            count = read(settings->shell_fd, buf, len - 1);
            buf[len - 1] = '\0'; // for strstr
            if (count < 0)
            {
                printf("read error: %s\n", strerror(errno));
                break;
            }
            if (count == 0)
                break;

            char *ptr = strstr(buf, "<END_TAG>");
            bool end = false;
            if (ptr != NULL)
            {
                end = true;
                count = ptr - buf;
            }

            fwrite(buf, 1, count, stdout);
            if (end)
            {
                break;
            }
        }

        if (errno == 5)
        {
            int fd = comm_setup();
            if (fd < 0) {
                log_error("Could not retry starting a shell.");
                goto end;
            }

            char exit_cmd[] = "\nexit\n";
            write(settings->shell_fd, exit_cmd, sizeof(exit_cmd));
            close(settings->shell_fd);
            settings->shell_fd = fd;
            log_verbose(settings->verbose, "Successfully recovered from lost connection with shell.");
            errno = 0;
        }
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
