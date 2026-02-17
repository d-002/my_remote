#include "stringutils.h"

void lower_str(char *s)
{
    for (size_t i = 0; s[i]; i++)
    {
        char c = s[i];
        if ('A' <= c && c <= 'Z')
        {
            s[i] = c + 'a' - 'A';
        }
    }
}
