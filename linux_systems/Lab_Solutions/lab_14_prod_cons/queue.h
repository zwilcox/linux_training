#ifndef QUEUE_H 
#define QUEUE_H 
 
typedef struct queue_node_t {
    struct queue_node_t* next;
    struct queue_node_t* prev;
    void* data;
} queue_node_t;

typedef struct queue_t {
    queue_node_t* head;
    queue_node_t* tail;
} queue_t;

queue_t* make_queue(void);
void free_queue(queue_t* queue);
int enqueue(queue_t* queue, void* data);
void* dequeue(queue_t* queue);

#endif
