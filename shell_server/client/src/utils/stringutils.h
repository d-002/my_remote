#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <stddef.h>
#include <string.h>

#define NULL_STRING ((struct string){ .data = NULL, .length = 0 })
#define STRING_FREE(string) free((string).data);

#define STREQL(s1, s2) (strcmp((s1), (s2)) == 0)
#define STRSTARTSWITH(string, prefix)                                          \
    (strlen(string) >= strlen(prefix)                                          \
     && memcmp(string, prefix, strlen(prefix)) == 0)

struct string
{
    char *data;
    size_t length;
};

void lower_str(char *s);

#endif /* ! STRINGUTILS_H */
