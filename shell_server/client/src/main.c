#include "settings/settings.h"
#include "settings/state.h"
#include "mainloop/mainloop.h"
#include "utils/sockutils.h"

int main(int argc, char *argv[])
{
    struct settings *settings = settings_create(argc, argv);
    struct state *state = state_create();
    if (settings == NULL || state == NULL) {
        settings_destroy(settings);
        state_destroy(state);
        return 1;
    }

    int res = mainloop(settings, state);

    settings_destroy(settings);
    state_destroy(state);
    return res;
    /*
    struct sock *sock = sock_create("localhost", "8080");
    if (sock == NULL)
    {
        return 1;
    }

    char str[] = "idle\n0.0.1\n";
    struct string content = { .data = str, .length = sizeof(str) - 1 };

    ssize_t count = sock_request(sock, "POST", "/api/heartbeat.php?user=d8e3141ff3d0fba630b86c8d4af6853cb930f29f67247907e87b21eeaafcbb31&machine=89a76cc7731845542079468858b7f0f4ec5d5d8c7d849842d14d16fcdc8bceb4", content);
    int res = count < 0 ? 1 : debug_print_recv(sock);

    sock_destroy(sock);
    return res;
    */
}
