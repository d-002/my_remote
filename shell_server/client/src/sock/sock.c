#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200112L
#endif /* ! _POSIX_C_SOURCE */

#include "sock.h"

#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#include "logger/logger.h"

#define SOCK_TIMEOUT_SECONDS 5

struct sock *sock_create(char *host, char *port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        log_error("Failed to create socket.");
        return NULL;
    }

    struct timeval tv;
    tv.tv_sec = SOCK_TIMEOUT_SECONDS;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    struct addrinfo hints;
    struct addrinfo *info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int status = getaddrinfo(host, port, &hints, &info);
    if (status)
    {
        log_error("Failed to get address info: %s.", gai_strerror(status));
        close(fd);
        return NULL;
    }

    int ok = 0;
    for (struct addrinfo *p = info; p; p = p->ai_next)
    {
        if (connect(fd, p->ai_addr, p->ai_addrlen) == -1)
        {
            continue;
        }

        ok = 1;
        break;
    }

    if (!ok)
    {
        log_error("Failed to connect.");
        freeaddrinfo(info);
        close(fd);
        return NULL;
    }

    struct sock *sock = malloc(sizeof(struct sock));
    size_t host_size = strlen(host) + 1;
    char *host_copy = malloc(host_size * sizeof(char));
    if (sock == NULL || host_copy == NULL)
    {
        log_alloc_error("sock create, copying to struct");
        close(fd);
        free(sock);
        free(host_copy);
        freeaddrinfo(info);
        return NULL;
    }

    memcpy(host_copy, host, host_size);
    sock->fd = fd;
    sock->host = host_copy;
    sock->info = info;

    return sock;
}

void sock_destroy(struct sock *sock)
{
    if (sock == NULL)
    {
        return;
    }

    close(sock->fd);
    free(sock->host);
    freeaddrinfo(sock->info);
    free(sock);
}
