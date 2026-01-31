#ifndef MACROS_H
#define MACROS_H

#define STREQL(s1, s2) (strcmp((s1), (s2)) == 0)
#define STRSTARTSWITH(string, prefix)                                          \
    (memcmp(string, prefix, strlen(prefix)) == 0)

#define SUCCESS 0
#define ERROR 1

#endif /* ! MACROS_H */
