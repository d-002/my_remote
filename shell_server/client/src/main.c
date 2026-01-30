#include "utils/sockutils.h"

int main(void) {
    struct sock *sock = sock_create("https://google.com", "80");
    sock_destroy(sock);
    return 0;
}
