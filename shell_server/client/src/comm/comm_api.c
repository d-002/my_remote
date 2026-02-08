#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include "comm_api.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "comm_setup.h"
#include "logger/logger.h"
#include "utils/errors.h"
#include "utils/stringbuilder.h"

#define BUF_SIZE 1024
#define END_TAG "<END_TAG>"

int send_sh(struct settings *settings, char *message)
{
    size_t length = strlen(message);

    while (length > 0)
    {
        ssize_t count = write(settings->shell_fd, message, length);
        if (count < 0)
        {
            log_error("Failed to send command to shell");
            return ERROR;
        }

        message += count;
        length -= count;
    }

    // add padding to know when the shell is done
    char padding[] = "\necho '" END_TAG "'\n";
    ssize_t count = write(settings->shell_fd, padding, sizeof(padding));
    if (count < 0)
    {
        log_error("Failed to send command padding to shell");
        return ERROR;
    }

    return SUCCESS;
}

int recv_sh(struct settings *settings, struct string *out)
{
    struct string_builder *sb = string_builder_create(NULL);
    if (sb == NULL)
    {
        return FATAL;
    }

    int err = SUCCESS;
    char buf[BUF_SIZE];

    size_t index = 0; // index while strstr_ing for the end tag
    while (1)
    {
        ssize_t count = read(settings->shell_fd, buf, BUF_SIZE - 1);
        if (count < 0)
        {
            log_error("Failed to read shell answer");
            break;
        }
        if (count == 0)
        {
            break;
        }

        buf[count] = '\0';
        bool end = false;
        for (size_t i = 0; i < (size_t)count; i++)
        {
            if (buf[i] == END_TAG[index])
            {
                if (++index == strlen(END_TAG))
                {
                    end = true;
                    count = i - strlen(END_TAG) + 1;
                    break;
                }
            }
            else
            {
                index = 0;
            }
        }

        char *copy = calloc(count + 1, sizeof(char));
        if (copy == NULL)
        {
            err = FATAL;
            log_alloc_error("duplicate shell response");
            break;
        }

        memcpy(copy, buf, count);
        err = string_builder_append(sb, copy);

        if (end || err != SUCCESS)
        {
            break;
        }
    }

    if (errno == 5)
    {
        err = restart_shell(settings);
        if (err == SUCCESS)
        {
            log_verbose(
                settings->verbose,
                "Successfully recovered from lost connection with shell.");
            errno = 0;
        }
        else
        {
            log_error("Could not retry starting a shell.");
        }
    }

    if (err == SUCCESS)
    {
        *out = string_builder_free_to_string(sb);
    }

    return err;
}
