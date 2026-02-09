#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200809L
#endif /* ! _POSIX_C_SOURCE */

#include "comm_api.h"

#include <errno.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "comm_setup.h"
#include "logger/logger.h"
#include "utils/errors.h"
#include "utils/stringbuilder.h"

#define BUF_SIZE 1024
#define FIRST_READ_TIMEOUT_MS 5000
#define READ_TIMEOUT_MS 500

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

    ssize_t count = write(settings->shell_fd, "\n", 1);
    if (count < 0)
    {
        log_error("Failed to send command postfix newline to shell");
        return ERROR;
    }

    return SUCCESS;
}

static void remove_cr(char *buf, size_t *count)
{
    size_t j = 0;
    for (size_t i = 0; i < *count; i++)
    {
        buf[j] = buf[i];
        char c = buf[i];
        j += c != '\r';
    }

    *count = j;
    buf[*count] = '\0';
}

/*
 * Add a sanitized command output to a stringbuilder.
 * Sanitized means there should be no empty lines nor leading / trailing spaces.
 */
static int sanitize_to_sb(char *buf, size_t count, struct string_builder *sb,
                          char *prev_c)
{
    remove_cr(buf, &count);

    if (count == 0)
    {
        return SUCCESS;
    }

    size_t start = 0;
    // If the last call ended in a newline, it will not have been added.
    // Add it now when adding the first line if it exists, to compensate.
    bool first_line = *prev_c != '\n';

    for (size_t i = 0; i <= count; i++)
    {
        // iterate one extra time to finish any eventual lines
        char c = i == count ? '\n' : buf[i];
        if (c != '\n')
            continue;

        // just ending a line, check if it was not empty
        if (i == start)
        {
            start = i + 1;
            continue;
        }

        // add a newline between lines if needed
        if (!first_line)
        {
            int err = string_builder_append(sb, "\n");
            if (err != SUCCESS)
            {
                return err;
            }
        }

        // add the line, but insert a terminating character
        buf[i] = '\0';
        int err = string_builder_append(sb, buf + start);
        if (err != SUCCESS)
        {
            return err;
        }
        buf[i] = c;

        first_line = false;
        start = i + 1;
    }

    *prev_c = buf[count - 1];
    return SUCCESS;
}

int recv_sh(struct settings *settings, struct string *out)
{
    // wait for data
    struct pollfd pfd;
    pfd.fd = settings->shell_fd;
    pfd.events = POLLIN;

    struct string_builder *sb = string_builder_create(NULL);
    if (sb == NULL)
    {
        return FATAL;
    }

    int err = SUCCESS;
    char buf[BUF_SIZE];
    char prev_c = '\0';

    bool has_read = false;
    while (1)
    {
        int res =
            poll(&pfd, 1, has_read ? READ_TIMEOUT_MS : FIRST_READ_TIMEOUT_MS);
        if (res < 0)
        {
            log_error("Failed to poll shell events.");
            err = ERROR;
            break;
        }

        if (res == 0)
        {
            if (!has_read)
            {
                log_error("Shell read timed out after %dms.", READ_TIMEOUT_MS);
                err = SUCCESS; // mark as success to skip reading this command
            }
            break;
        }

        ssize_t count = read(settings->shell_fd, buf, BUF_SIZE - 1);
        if (count < 0)
        {
            log_error("Failed to read shell answer");
            err = ERROR;
            break;
        }
        if (count == 0)
        {
            break;
        }

        has_read = true;
        buf[count] = '\0';

        err = sanitize_to_sb(buf, count, sb, &prev_c);
        if (err != SUCCESS)
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
    else
    {
        string_builder_destroy(sb);
    }

    return err;
}
