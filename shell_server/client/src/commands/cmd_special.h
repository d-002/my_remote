#ifndef CMD_SPECIAL_H
#define CMD_SPECIAL_H

#include <stdbool.h>

#include "logger/logger.h"
#include "settings/settings.h"

int restart(struct settings *settings);

int send_report(struct settings *settings, log_func log_func, char *report);
int destroy(struct settings *settings);

#endif /* ! CMD_SPECIAL_H */
