#include <stdio.h>

#include "utils/sockutils.h"

int main(void)
{
    struct sock *sock = sock_create("localhost", "8080");
    if (sock == NULL)
    {
        return 1;
    }

    struct string content = { .data = "hello world", .length = 12 };
    ssize_t count = sock_request(sock, "GET", "/", content);

    char buf[64];
    size_t length = 0;
    while (1)
    {
        ssize_t count = recv(sock->fd, buf, 63, 0);
        if (count < 0)
        {
            printf("error in recv\n");
            break;
        }
        if (count == 0)
        {
            break;
        }

        length += count;
        buf[count] = '\0';
        printf("%s", buf);
    }

    sock_destroy(sock);
    return count < 0;
}
