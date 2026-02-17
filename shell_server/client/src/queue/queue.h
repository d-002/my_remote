#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

typedef void (*free_func)(void *ptr);

struct queue_node
{
    void *data;
    struct queue_node *next;
};

/**
 * Generic queue.
 * These pointers should be NULL or point to heap-allocated data.
 * The data will be freed with the given free function when the queue is
 * destroyed.
 * They are obviously NOT freed when dequeued.
 */
struct queue
{
    struct queue_node *head;
    struct queue_node *tail;

    free_func free_func;

    size_t length;
};

struct queue *queue_create(free_func free_func);
void queue_destroy(struct queue *queue);

/*
 * Remove and free all elements of the queue.
 */
void queue_clear(struct queue *queue);

int queue_enqueue(struct queue *queue, void *data);
void *queue_dequeue(struct queue *queue);

#endif /* ! QUEUE_H */
