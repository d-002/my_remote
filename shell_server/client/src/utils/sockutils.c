#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200112L
#endif /* ! _POSIX_C_SOURCE */

#include "sockutils.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

struct sock *sock_create(char *addr, char *port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        return NULL;
    }

    struct addrinfo hints;
    struct addrinfo *info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int status = getaddrinfo(addr, port, &hints, &info);
    if (status)
    {
        close(fd);
        return NULL;
    }

    int ok = 0;
    for (struct addrinfo *p = info; p; p = p->ai_next) {
        if (connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
            continue;
        }

        ok = 1;
        break;
    }

    if (!ok) {
        close(fd);
        return NULL;
    }

    struct sock *sock = malloc(sizeof(struct sock));
    if (sock == NULL)
    {
        return NULL;
    }

    sock->fd = fd;
    sock->info = info;

    return sock;
}

void sock_destroy(struct sock *sock)
{
    if (sock == NULL)
    {
        return;
    }

    freeaddrinfo(sock->info);
    close(sock->fd);
    free(sock);
}
