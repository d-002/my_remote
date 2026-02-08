#ifndef COMMAND_H
#define COMMAND_H

#include <stdbool.h>
#include <stddef.h>

enum command_sender
{
    USER,
    MACHINE,
};

struct command
{
    char *content;
    enum command_sender sender;
    bool is_read;
};

struct command *command_create(char *stream, size_t length);
void command_destroy(void *ptr);

#endif /* ! COMMAND_H */
