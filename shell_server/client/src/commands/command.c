#include "command.h"

#include <stdlib.h>
#include <string.h>

struct command *command_create(char *stream, size_t length)
{
    struct command *command = malloc(sizeof(struct command));
    if (command == NULL)
    {
        return NULL;
    }

    // check command status with the first few words

    if (memcmp(stream, "user", 4) == 0)
        command->sender = USER;
    else
        command->sender = MACHINE;

    char *ptr = strchr(stream, ' ') + 1;
    if (ptr == NULL) {
        free(command);
        return NULL;
    }

    length -= ptr - stream;
    stream = ptr;
    command->is_read = memcmp(stream, "read", 4) == 0;

    // copy command content

    ptr = strchr(stream, ':') + 2;
    if (ptr == NULL) {
        free(command);
        return NULL;
    }

    length -= ptr - stream;
    stream = ptr;

    command->content = calloc(length + 1, sizeof(char));
    if (command->content == NULL)
    {
        free(command);
        return NULL;
    }

    memcpy(command->content, stream, length);

    return command;
}

void command_destroy(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    struct command *command = ptr;
    free(command->content);
    free(command);
}
