#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

static queue_node_t* make_queue_node(void* data);

/**
 * Create a new empty queue
 * @returns queue pointer or NULL on failure
 */
queue_t* make_queue() {
    queue_t* queue;
    queue = (queue_t*)malloc(sizeof(queue_t));
    if (queue == NULL) {
        fprintf(stderr, "Failed to allocate space for queue\n");
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}

/**
 * Empties queue and frees all nodes, and frees queue
 * @param queue pointer to the qeueue to free
 */
void free_queue(queue_t* queue) {
    queue_node_t* current;
    queue_node_t* next;
    if (queue == NULL) {
        return;
    }
    current = queue->head;
    while (current != NULL) {
        next = current->next;
        /* We're not freeing the contents of nodes on purpose.
           It was created by others and is their responsibility. */
        free(current);
        current = next;
    }
    free(queue);
    return;
}

/**
 * Creates a queue node
 * @param data pointer to the data to enqueue
 * @returns queue_node_t pointer or NULL on failure
 */
queue_node_t* make_queue_node(void* data) {
    queue_node_t* node;
    node = (queue_node_t*)malloc(sizeof(queue_node_t));
    if (node == NULL) {
        return NULL;
    }
    node->next = NULL;
    node->prev = NULL;
    node->data = data;
    return node;
}

/**
 * Adds an item to the specified queue
 * @param queue pointer to queue to hold data
 * @param data pointer to data to place in queue
 * @returns 1 on success, 0 on failure
 */
int enqueue(queue_t* queue, void* data) {
    queue_node_t* new_node;
    new_node = make_queue_node(data);
    if (new_node == NULL) {
        return 0;
    }

    if (queue->head == NULL) {
        queue->head = new_node;
        queue->tail = new_node;
        return 1;
    }

    new_node->prev = queue->tail;
    queue->tail->next = new_node;
    queue->tail = new_node;
    return 1;
}

/**
 * Returns the first element on the queue and removes it
 * @param queue pointer to queue holding element
 * @returns pointer to first item or NULL if queue is empty
 */
void* dequeue(queue_t* queue) {
    void* data;
    queue_node_t* node;
    if (queue->head == NULL) {
        return NULL;
    }
    node = queue->head;
    data = node->data;

    if (queue->tail == node) {
        queue->tail = NULL;
    }

    queue->head = node->next;
    if (queue->head != NULL) {
        queue->head->prev = NULL;
    }
    free(node);
    return data;
}
