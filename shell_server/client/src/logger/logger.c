#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

void log_inner(FILE *stream, const char *prefix, const char *format,
               va_list args)
{
    fputs(prefix, stream);
    fputs("shell_client: ", stream);

    vfprintf(stream, format, args);
    putc('\n', stream);
}

void log_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner(stdout, "[INFO]  ", format, args);
    va_end(args);
}

void log_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_inner(stderr, "[ERROR] ", format, args);
    va_end(args);
}

void log_verbose(bool verbose, const char *format, ...) {
    if (!verbose) {
        return;
    }

    va_list args;
    va_start(args, format);
    log_inner(stderr, "[VERBOSE] ", format, args);
    va_end(args);
}

void log_alloc_error()
{
    log_error("Failed to allocate memory.");
}
