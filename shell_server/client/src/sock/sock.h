#ifndef SOCK_H
#define SOCK_H

struct sock
{
    int fd;
    char *host; // allocated copy of the parameter
    struct addrinfo *info;
};

struct sock *sock_create(char *addr, char *port);
void sock_destroy(struct sock *sock);

#endif /* ! SOCK_H */
