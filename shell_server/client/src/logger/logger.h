#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

#define NO_LINE -1

void log_info(const char *format, ...);
void log_error(const char *format, ...);
void log_verbose(bool verbose, const char *format, ...);
void log_alloc_error(const char *where);

#endif /* ! LOGGER_H */
