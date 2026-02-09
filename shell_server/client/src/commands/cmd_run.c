#include "cmd_run.h"

#include <stdio.h>
#include <stdlib.h>

#include "cmd_special.h"
#include "comm/comm_api.h"
#include "command.h"
#include "logger/logger.h"
#include "sock/sockutils.h"
#include "utils/errors.h"

#define FALLBACK_STRING ""

static int send_command_output(struct settings *settings, struct string output)
{
    char *url_arr[] = {
        "/api/enqueue_command.php?user=",
        settings->user_hash,
        "&machine=",
        settings->machine_hash,
        NULL,
    };

    // copy output.data because it could get overriden by a fallback string in
    // case there was no output
    char *ptr = output.data;
    if (output.data == NULL)
    {
        output.data = FALLBACK_STRING;
    }

    printf("%s\n", output.data);

    // need to encode newlines to not pollute the output
    for (size_t i = 0; i < output.length; i++)
    {
        char c = output.data[i];
        if (c != '\n')
        {
            continue;
        }

        output.data[i] = 0x7; // let's hope this character (bell) is never used
    }

    struct string response = NULL_STRING;
    int err = post_wrapper(settings, url_arr, output, &response);
    output.data = ptr;
    STRING_FREE(response);
    return err;
}

static int mark_command_as_read(struct settings *settings)
{
    char *url_arr[] = {
        "/api/machine_read_command.php?user=",
        settings->user_hash,
        "&machine=",
        settings->machine_hash,
        NULL,
    };

    struct string response = NULL_STRING;
    int err = post_wrapper(settings, url_arr, NULL_STRING, &response);
    STRING_FREE(response);
    return err;
}

static int run_command(struct settings *settings, struct command *command,
                       bool *action)
{
    if (command->is_read || command->sender != USER)
        return SUCCESS;

    printf("$ %s\n", command->content);
    *action = true;

    int err = send_sh(settings, command->content);
    if (err != SUCCESS)
    {
        return err;
    }

    struct string out = NULL_STRING;
    err = recv_sh(settings, &out);

    if (err == SUCCESS)
    {
        err = send_command_output(settings, out);
        if (err == SUCCESS)
        {
            err = mark_command_as_read(settings);
        }
        STRING_FREE(out);
    }

    return err;
}

static int run_special(struct settings *settings, struct command *command,
                       bool *action)
{
    if (command->is_read || command->sender != USER)
        return SUCCESS;

    int err = SUCCESS;
    if (STREQL(command->content, "restart"))
    {
        *action = true;
        err = restart(settings);
    }
    else if (STREQL(command->content, "destroy"))
    {
        *action = true;
        err = destroy(settings);
    }
    else
    {
        return send_report(settings, log_error, "Unknown special action type.");
    }

    if (err == SUCCESS || err == EXIT)
    {
        if (err == SUCCESS)
        {
            err = mark_command_as_read(settings);
        }
    }

    return err;
}

int run_all_commands(struct settings *settings, struct queue *queue,
                     bool *action)
{
    for (struct command *command = queue_dequeue(queue); command;
         command = queue_dequeue(queue))
    {
        int err = command->is_special ? run_special(settings, command, action)
                                      : run_command(settings, command, action);
        command_destroy(command);

        if (err == FATAL || err == EXIT)
        {
            return err;
        }
    }

    return SUCCESS;
}
