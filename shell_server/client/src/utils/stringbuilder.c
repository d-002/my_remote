#include "stringbuilder.h"

#include <stdlib.h>
#include <string.h>

#include "logger/logger.h"
#include "utils/macros.h"

struct string_builder *string_builder_create(char *s)
{
    struct string_builder *sb = malloc(sizeof(struct string_builder));
    if (sb == NULL)
    {
        return NULL;
    }

    if (s == NULL)
    {
        sb->data = NULL;
        sb->length = 0;
    }
    else
    {
        sb->length = strlen(s);
        sb->data = malloc((sb->length + 1) * sizeof(char));

        if (sb->data == NULL)
        {
            log_alloc_error();
            free(sb);
            return NULL;
        }

        memcpy(sb->data, s, sb->length + 1);
    }

    return sb;
}

void string_builder_destroy(struct string_builder *sb)
{
    if (sb == NULL)
    {
        return;
    }

    free(sb->data);
    free(sb);
}

int string_builder_append(struct string_builder *sb, char *s)
{
    size_t len_add = strlen(s);
    size_t len = sb->length + len_add;
    char *dst = realloc(sb->data, len + 1);
    if (dst == NULL)
    {
        log_alloc_error();
        return ERROR;
    }

    memcpy(dst + sb->length, s, (len_add + 1) * sizeof(char));
    sb->data = dst;

    return SUCCESS;
}

char *string_builder_detach(struct string_builder *sb)
{
    char *s = sb->data;
    sb->data = NULL;
    sb->length = 0;

    return s;
}

struct string concat_str(char *arr[])
{
    if (arr == NULL || arr[0] == NULL)
    {
        return NULL_STRING;
    }

    struct string_builder *sb = string_builder_create(arr[0]);
    if (sb == NULL)
    {
        return NULL_STRING;
    }

    for (size_t i = 1; arr[i]; i++)
    {
        int err = string_builder_append(sb, arr[i]);
        if (err != SUCCESS)
        {
            string_builder_destroy(sb);
            return NULL_STRING;
        }
    }

    char *s = string_builder_detach(sb);
    size_t length = sb->length;
    string_builder_destroy(sb);

    return (struct string){ .data = s, .length = length };
}
