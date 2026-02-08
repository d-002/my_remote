#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */
#ifndef _DEFAULT_SOURCE
#    define _DEFAULT_SOURCE
#endif /* ! _DEFAULT_SOURCE */

#include "comm_setup.h"

#include <pty.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#include "logger/logger.h"
#include "utils/errors.h"

#define SHELL "/bin/sh"

int comm_setup(struct settings *settings)
{
    int master;
    if (setenv("TERM", "dumb", 1)) {
        log_error("Failed to set TERM=dumb env variable.");
        return FATAL;
    }

    int pid = forkpty(&master, NULL, NULL, NULL);

    if (pid < 0)
    {
        log_error("forkpty failed.");
        return -1;
    }

    // child, run the shell
    if (pid == 0)
    {
        char *argv[] = {
            SHELL,
            NULL,
        };
        // Yeah I leak system resources by making this a zombie. Who cares?
        execvp(argv[0], argv);

        log_info("Should not happen. Code running after call to execvp.");
        return -1;
    }

    struct termios tios;
    tcgetattr(master, &tios);
    cfmakeraw(&tios);
    tcsetattr(master, TCSANOW, &tios);

    // silence the shell
    char silence[] =
        "PS1='';PS2='';PS3='';PROMPT='';export PS1 PS2 PS3 PROMPT\n";
    write(master, silence, strlen(silence));

    log_verbose(settings->verbose, "Successfully set up shell process.");

    return master;
}

int restart_shell(struct settings *settings)
{
    int fd = comm_setup(settings);
    if (fd < 0)
        return FATAL;

    char exit_cmd[] = "\nexit\n";
    write(settings->shell_fd, exit_cmd, strlen(exit_cmd));
    close(settings->shell_fd);
    settings->shell_fd = fd;

    return SUCCESS;
}
