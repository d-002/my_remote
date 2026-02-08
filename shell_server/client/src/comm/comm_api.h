#ifndef COMM_API_H
#define COMM_API_H

#include "settings/settings.h"
#include "utils/stringutils.h"

int send_sh(struct settings *settings, char *message);
int recv_sh(struct settings *settings, struct string *out);

#endif /* ! COMM_API_H */
