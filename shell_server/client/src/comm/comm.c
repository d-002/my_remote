#include "comm.h"

#include <stdlib.h>
#include <unistd.h>

#include "logger/logger.h"
#include "utils/errors.h"

#define BASE_SHELL "/bin/sh"

char *get_shell()
{
#ifdef _WIN32
    return getenv("COMSPEC");
#else
    char *shell = getenv("SHELL");
    return shell ? shell : BASE_SHELL;
#endif
}

int comm_setup(struct settings *settings, int fd[2])
{
    char *shell = get_shell();
    if (shell == NULL || shell[0] == '\0')
    {
        log_error("Failed to find a shell");
        return FATAL;
    }

    log_verbose(settings->verbose, "Using '%s' as a shell", shell);

    int stdin_pipe[2];
    int stdout_pipe[2];

    if (pipe(stdin_pipe) < 0)
    {
        log_error("Failed to create stdin pipe");
        return FATAL;
    }
    if (pipe(stdout_pipe) < 0)
    {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        log_error("Failed to create stdout pipe");
        return FATAL;
    }

    int pid = fork();

    if (pid < 0)
    {
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        log_error("Failed to fork and launch a shell");
        return FATAL;
    }

    // child, run the shell
    if (pid == 0)
    {
        dup2(stdin_pipe[0], STDIN_FILENO);
        dup2(stdout_pipe[1], STDOUT_FILENO);
        dup2(stdout_pipe[1], STDERR_FILENO);

        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);

        char *argv[] = {
            "script",
            "-q",
            "/dev/null",
            "-c",
            shell,
            NULL,
        };
        // Yeah I leak system resources by making this a zombie. Who cares?
        execvp(argv[0], argv);

        log_info("Should not happen. Code running after call to execvp.");
        return ERROR;
    }

    // parent, continue with the rest of the program
    close(stdin_pipe[0]);
    close(stdout_pipe[1]);
    fd[1] = stdin_pipe[1];
    fd[0] = stdout_pipe[0];

    return SUCCESS;
}
