#ifndef CMD_RUN_H
#define CMD_RUN_H

#include <stdbool.h>

#include "queue/queue.h"
#include "settings/settings.h"

int run_all_commands(struct settings *settings, struct queue *queue,
                     bool *action);

#endif /* ! CMD_RUN_H */
