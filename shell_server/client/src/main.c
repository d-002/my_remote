#include "utils/sockutils.h"

int main(void)
{
    struct sock *sock = sock_create("google.com", "80");
    if (sock == NULL)
    {
        return 1;
    }

    struct string content = { .data = "hello world", .length = 12 };
    ssize_t count = sock_request(sock, "GET", "/", content);

    sock_destroy(sock);
    return count < 0;
}
