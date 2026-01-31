#ifndef _POSIX_C_SOURCE
#    define _POSIX_C_SOURCE 200112L
#endif /* ! _POSIX_C_SOURCE */

#include "sockutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include "errors.h"
#include "logger/logger.h"

#define HEADERS_SIZE 1024
#define LINE_SIZE 128

struct sock *sock_create(char *host, char *port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        log_error("Failed to create socket.");
        return NULL;
    }

    struct addrinfo hints;
    struct addrinfo *info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int status = getaddrinfo(host, port, &hints, &info);
    if (status)
    {
        log_error("Failed to get address info.");
        close(fd);
        return NULL;
    }

    int ok = 0;
    for (struct addrinfo *p = info; p; p = p->ai_next)
    {
        if (connect(fd, p->ai_addr, p->ai_addrlen) == -1)
        {
            continue;
        }

        ok = 1;
        break;
    }

    if (!ok)
    {
        log_error("Failed to connect.");
        freeaddrinfo(info);
        close(fd);
        return NULL;
    }

    struct sock *sock = malloc(sizeof(struct sock));
    size_t host_size = strlen(host) + 1;
    char *host_copy = malloc(host_size * sizeof(char));
    if (sock == NULL || host_copy == NULL)
    {
        log_alloc_error();
        close(fd);
        free(sock);
        free(host_copy);
        freeaddrinfo(info);
        return NULL;
    }

    memcpy(host_copy, host, host_size);
    sock->fd = fd;
    sock->host = host_copy;
    sock->info = info;

    return sock;
}

void sock_destroy(struct sock *sock)
{
    if (sock == NULL)
    {
        log_alloc_error();
        return;
    }

    close(sock->fd);
    free(sock->host);
    freeaddrinfo(sock->info);
    free(sock);
}

ssize_t sock_request(struct sock *sock, char *request_type, char *path,
                     struct string content)
{
    char headers[HEADERS_SIZE];

    char content_length_line[LINE_SIZE];
    int is_post = STREQL(request_type, "POST");
    if (is_post)
    {
        snprintf(content_length_line, LINE_SIZE, "Content-Length: %ld\r\n",
                 content.length);
    }
    else
    {
        content_length_line[0] = '\0';
    }

    snprintf(headers, HEADERS_SIZE,
             "%s %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: keep-alive\r\n"
             "%s\r\n",
             request_type, path, sock->host, content_length_line);

    ssize_t total = send(sock->fd, headers, strlen(headers), 0);

    if (total < 0)
    {
        log_error("Failed to send headers.");
        return total;
    }

    if (is_post)
    {
        size_t index = 0;
        while (index < content.length)
        {
            ssize_t add =
                send(sock->fd, content.data + index, content.length, 0);
            if (add < 0)
            {
                log_error("Failed to send part of the POSTed content.");
                return add;
            }
            if (add == 0)
            {
                break;
            }

            index += add;
            total += add;
        }
    }

    return total;
}

static int update_content_length(struct string line, int *found, ssize_t *out)
{
    log_info("'%s'", line.data);
    char *ptr = strchr(line.data, ':');
    if (ptr == NULL)
    {
        log_error("Could not find colon in header line.");
        return ERROR;
    }

    size_t index = ptr - line.data;

    lower_str(line.data);

    if (STRSTARTSWITH(line.data, "content-length"))
    {
        *found = 1;
        *out = atoi(line.data + index + (line.data[index + 1] == ' '));
    }

    return SUCCESS;
}

static int find_content_length(struct sock *sock,
                               struct string_builder *content_sb,
                               struct string_builder *line_sb, ssize_t *out)
{
    char buf[LINE_SIZE];
    int first_line = 1;
    int found = 0;

    bool done = false;
    while (!done)
    {
        ssize_t count = recv(sock->fd, buf, LINE_SIZE - 1, 0);
        if (count < 0)
        {
            log_error("Could not read from socket.");
            return ERROR;
        }
        if (count == 0)
        {
            break;
        }

        buf[count] = '\0';

        size_t start = 0;
        for (size_t i = 0; i <= (size_t)count && !done; i++)
        {
            char c = buf[i];

            // ending a line
            if (c == '\n')
            {
                buf[i] = '\0'; // artificial end of string for append
                int res = string_builder_append(line_sb, buf + start);

                if (res != SUCCESS)
                {
                    return res;
                }

                struct string line = string_builder_detach(line_sb);

                // check for a \r\n\r\n indicating the end of the headers
                if (line.length == 1 && line.data[0] == '\r')
                {
                    done = true;
                }
                else if (first_line)
                {
                    first_line = 0;
                }
                else
                {
                    res = update_content_length(line, &found, out);
                    if (res != SUCCESS)
                    {
                        return res;
                    }
                }

                STRING_FREE(line);
                buf[i] = c;
                start = i + 1;
            }
        }

        // write the remaining non-line bytes into the line sb for the next
        // iteration, or all the remaining bytes into the content sb
        if (start < LINE_SIZE - 1)
        {
            int res =
                string_builder_append(done ? content_sb : line_sb, buf + start);
            if (res != SUCCESS)
            {
                return res;
            }
        }
    }

    if (!found)
    {
        log_error("Could not find content_length in response, assuming only "
                  "one packet");
    }
    return SUCCESS;
}

static int reconstitute_content(struct sock *sock,
                                struct string_builder *content_sb,
                                ssize_t content_length)
{
    char buf[LINE_SIZE];
    ssize_t total_read = 0;

    while (content_length == -1 || total_read < content_length)
    {
        ssize_t count = recv(sock->fd, buf, LINE_SIZE - 1, 0);
        if (count < 0)
        {
            log_error("Could not read from socket.");
            return ERROR;
        }
        if (count == 0)
        {
            break;
        }

        buf[count] = '\0';

        log_info("reading %ld bytes", count);
        total_read += count;

        int res = string_builder_append(content_sb, buf);
        if (res != SUCCESS)
        {
            return res;
        }
    }

    return SUCCESS;
}

struct string recv_content(struct sock *sock)
{
    struct string_builder *content_sb = string_builder_create(NULL);
    struct string_builder *line_sb = string_builder_create(NULL);
    if (content_sb == NULL || line_sb == NULL)
    {
        goto error;
    }

    ssize_t content_length = -1;
    int err = find_content_length(sock, content_sb, line_sb, &content_length);
    if (err != SUCCESS)
    {
        goto error;
    }

    log_info("found content length of %d", content_length);
    log_info("current content length is %ld", content_sb->length);

    err = reconstitute_content(sock, content_sb, content_length);
    if (err != SUCCESS)
    {
        goto error;
    }

    string_builder_destroy(line_sb);
    return string_builder_free_to_string(content_sb);

error:
    string_builder_destroy(content_sb);
    string_builder_destroy(line_sb);
    return NULL_STRING;
}
