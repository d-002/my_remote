#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#include "stringutils.h"

struct string_builder
{
    char *data;
    size_t length;
};

struct string_builder *string_builder_create(char *s);
void string_builder_destroy(struct string_builder *sb);

int string_builder_append(struct string_builder *sb, char *s);
struct string string_builder_detach(struct string_builder *sb);

struct string concat_str(char *arr[]);
struct string string_builder_free_to_string(struct string_builder *sb);

#endif /* ! STRINGBUILDER_H */
