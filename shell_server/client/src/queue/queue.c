#include "queue.h"

#include <stdlib.h>

#include "utils/errors.h"

/*
 * Set all fields of `queue` to their default value.
 * `queue->free_func` is left uninitialized.
 */
static void queue_init(struct queue *queue)
{
    queue->head = NULL;
    queue->tail = NULL;
    queue->length = 0;
}

struct queue *queue_create(free_func free_func)
{
    struct queue *queue = malloc(sizeof(struct queue));
    if (queue == NULL)
    {
        return NULL;
    }

    queue_init(queue);

    queue->free_func = free_func;
    return queue;
}

void queue_destroy(struct queue *queue)
{
    if (queue == NULL)
    {
        return;
    }

    queue_clear(queue);
    free(queue);
}

void queue_clear(struct queue *queue)
{
    struct queue_node *node = queue->head;
    while (node)
    {
        struct queue_node *next = node->next;
        queue->free_func(node->data);
        free(node);
        node = next;
    }
    queue_init(queue);
}

int queue_enqueue(struct queue *queue, void *data)
{
    struct queue_node *node = malloc(sizeof(struct queue_node));
    if (node == NULL)
    {
        return FATAL;
    }

    node->data = data;
    node->next = NULL;

    if (queue->tail == NULL)
    {
        queue->head = node;
    }
    else
    {
        queue->tail->next = node;
    }

    queue->tail = node;
    queue->length++;

    return SUCCESS;
}

void *queue_dequeue(struct queue *queue)
{
    if (queue->head == NULL)
    {
        return NULL;
    }

    struct queue_node *node = queue->head;
    queue->head = node->next;
    if (node == queue->tail)
    {
        queue->tail = NULL;
    }

    void *res = node->data;
    free(node);

    queue->length--;
    return res;
}
