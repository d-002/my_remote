#include "sockutils.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

#include "sock.h"
#include "utils/errors.h"
#include "utils/stringbuilder.h"
#include "logger/logger.h"

#define HEADERS_SIZE 1024
#define LINE_SIZE 128

struct sock *sock_request(struct settings *settings, char *request_type, char *path, struct string content)
{
    struct sock *sock = sock_create(settings->host, settings->port);
    if (sock == NULL) {
        return NULL;
    }

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

    ssize_t count = send(sock->fd, headers, strlen(headers), 0);

    if (count < 0)
    {
        log_error("Failed to send headers.");
        sock_destroy(sock);
        return NULL;
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
                sock_destroy(sock);
                return NULL;
            }
            if (add == 0)
            {
                break;
            }

            index += add;
        }
    }

    return sock;
}

static int update_content_length(struct string line, int *found, ssize_t *out)
{
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
                int err = string_builder_append(line_sb, buf + start);

                if (err != SUCCESS)
                {
                    return err;
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
                    err = update_content_length(line, &found, out);
                    if (err != SUCCESS)
                    {
                        return err;
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
            int err =
                string_builder_append(done ? content_sb : line_sb, buf + start);
            if (err != SUCCESS)
            {
                return err;
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

        total_read += count;

        int err = string_builder_append(content_sb, buf);
        if (err != SUCCESS)
        {
            return err;
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
