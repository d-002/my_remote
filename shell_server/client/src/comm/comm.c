#include "comm.h"

#include <pty.h>
#include <termios.h>
#include <unistd.h>

#include "logger/logger.h"

#define SHELL "/bin/sh"

int comm_setup()
{
    int master;
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

    // silence the shell
    char silence[] =
        "PS1='';PS2='';PS3='';PROMPT='';export PS1 PS2 PS3 PROMPT\n";
    write(master, silence, sizeof(silence));

    struct termios tios;
    tcgetattr(master, &tios);
    tios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
    tcsetattr(master, TCSANOW, &tios);

    return master;
}
