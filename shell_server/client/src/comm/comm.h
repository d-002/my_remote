#ifndef COMM_H
#define COMM_H

#include "settings/settings.h"

int comm_setup(struct settings *settings, int fd[2]);

#endif /* ! COMM_H */
