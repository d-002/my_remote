#ifndef SOCKUTILS_H
#define SOCKUTILS_H

#include <stddef.h>

#include "utils/stringutils.h"
#include "settings/settings.h"

/*
 * Send a request to the given socket.
 * Supported request types are GET and POST, undefined behavior otherwise.
 * In GET requests, the content argument's string field may be set to NULL.
 * In POST requests, it will be send()ed after the main headers.
 */
struct sock *sock_request(struct settings *settings, char *request_type,
                          char *path, struct string content);

// will be assuming the request newlines are correctly formatted
struct string recv_content(struct sock *sock);

int post_wrapper(struct settings *settings, char *url_arr[], struct string content, struct string *out);

#endif /* ! SOCKUTILS_H */
