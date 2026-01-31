#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#include <stddef.h>
#include <string.h>

#define NULL_STRING ((struct string){ .data = NULL, .length = 0 })

// made to combine a string and its length
struct string
{
    char *data;
    size_t length;
};

// made to grow
struct string_builder
{
    char *data;
    size_t length;
};

struct string_builder *string_builder_create(char *s);
void string_builder_destroy(struct string_builder *sb);

int string_builder_append(struct string_builder *sb, char *s);
char *string_builder_detach(struct string_builder *sb);

struct string concat_str(char *arr[]);

#endif /* ! STRINGBUILDER_H */
