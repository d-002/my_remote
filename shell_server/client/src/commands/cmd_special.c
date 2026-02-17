#include "cmd_special.h"

#include <libgen.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sock/sockutils.h"
#include "utils/errors.h"
#include "utils/stringutils.h"

int restart(struct settings *settings)
{
    send_report(settings, log_info, "Restarting in a new process.");

    int pid = fork();

    if (pid < 0)
    {
        log_info("Failed to start new process.");
        return ERROR;
    }

    if (pid == 0)
    {
        execvp(settings->argv[0], settings->argv);

        log_info("Should not happen. Code running after call to execvp.");
        return ERROR;
    }

    return EXIT;
}

int destroy(struct settings *settings)
{
    log_info("Removing self...");

    char *files[] = {
        settings->argv[0], HOST_FILE,    PORT_FILE, USER_HASH_FILE,
        MACHINE_HASH_FILE, VERSION_FILE, NULL,
    };

    for (size_t i = 0; files[i]; i++)
    {
        log_verbose(settings->verbose, "Removing '%s'", files[i]);
        if (remove(files[i]) < 0)
        {
            log_error("Failed to remove '%s'", files[i]);
        }
    }

    // also delete parent directory if empty (if not empty, will trigger an
    // error, ignore it)
    char cwd[PATH_MAX];
    if (getcwd(cwd, PATH_MAX) != NULL)
    {
        char *parent = dirname(cwd);
        if (parent != NULL)
        {
            rmdir(parent);
        }
    }

    send_report(settings, log_info, "Done destroying, will stop now.");
    return EXIT;
}

int send_report(struct settings *settings, log_func log_func, char *report)
{
    log_func(report);

    char *url_arr[] = {
        "/api/enqueue_command.php?user=", settings->user_hash, "&machine=",
        settings->machine_hash,           "&status=report",    NULL,
    };

    struct string content = {
        .data = report,
    };
    content.length = strlen(content.data);

    struct string out = NULL_STRING;
    int err = post_wrapper(settings, url_arr, content, &out);
    STRING_FREE(out);
    return err;
}
