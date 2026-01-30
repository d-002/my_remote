#ifndef SOCKUTILS_H
#define SOCKUTILS_H

#include <netdb.h>

struct sock {
    int fd;
    struct addrinfo *info;
};

struct sock *sock_create(char *addr, char *port);
void sock_destroy(struct sock *sock);

#endif /* ! SOCKUTILS_H */
