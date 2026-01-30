#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200112L
#endif /* ! _POSIX_C_SOURCE */

#include "sockutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "logger/logger.h"
#include "macros.h"

#define HEADERS_SIZE 1024
#define LINE_SIZE 128

struct sock *sock_create(char *host, char *port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        logerror("Failed to create socket.");
        return NULL;
    }

    struct addrinfo hints;
    struct addrinfo *info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int status = getaddrinfo(host, port, &hints, &info);
    if (status)
    {
        logerror("Failed to get address info.");
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
        logerror("Failed to connect.");
        freeaddrinfo(info);
        close(fd);
        return NULL;
    }

    struct sock *sock = malloc(sizeof(struct sock));
    size_t host_size = strlen(host) + 1;
    char *host_copy = malloc(host_size * sizeof(char));
    if (sock == NULL || host_copy == NULL)
    {
        log_alloc_error();
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
        log_alloc_error();
        return;
    }

    close(sock->fd);
    free(sock->host);
    freeaddrinfo(sock->info);
    free(sock);
}

ssize_t sock_request(struct sock *sock, char *request_type, char *path,
                     struct string content)
{
    char headers[HEADERS_SIZE];

    char content_length_line[LINE_SIZE];
    int is_post = STREQL(request_type, "POST");
    if (is_post)
    {
        snprintf(content_length_line, LINE_SIZE, "Content-Length: %ld\r\n", content.length);
    }
    else {
        content_length_line[0] = '\0';
    }

    snprintf(headers, HEADERS_SIZE,
             "%s %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "%s\r\n",
             request_type, path, sock->host, content_length_line);

    ssize_t total = send(sock->fd, headers, strlen(headers), 0);

    if (total < 0)
    {
        logerror("Failed to send headers.");
        return total;
    }

    if (is_post)
    {
        size_t index = 0;
        while (index < content.length)
        {
            ssize_t add = send(sock->fd, content.data + index, content.length, 0);
            if (add < 0)
            {
                logerror("Failed to send part of the POSTed content.");
                return add;
            }
            if (add == 0)
            {
                break;
            }

            index += add;
            total += add;
        }
    }

    return total;
}
