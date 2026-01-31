#ifndef SOCKUTILS_H
#define SOCKUTILS_H

#include <netdb.h>
#include <stddef.h>

#include "stringutils.h"

struct sock
{
    int fd;
    char *host; // allocated copy of the parameter
    struct addrinfo *info;
};

struct sock *sock_create(char *addr, char *port);
void sock_destroy(struct sock *sock);

/*
 * Send a request to the given socket.
 * Supported request types are GET and POST, undefined behavior otherwise.
 * In GET requests, the content argument's string field may be set to NULL.
 * In POST requests, it will be send()ed after the main headers.
 */
ssize_t sock_request(struct sock *sock, char *request_type, char *path,
                     struct string content);

int debug_print_recv(struct sock *sock);

#endif /* ! SOCKUTILS_H */
