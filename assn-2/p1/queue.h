#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>

#include "pcb.h"

typedef struct queue_node queue_node_t;
typedef struct queue queue_t;

struct queue_node {
    pcb_t *pcb;
    queue_node_t *next;
};

struct queue {
    int size;
    queue_node_t *head, *tail;
};

int init_queue(queue_t **q);
int size(queue_t *q);
bool empty(queue_t *q);
int front(queue_t *q, pcb_t **p);
int enqueue(queue_t *q, pcb_t *p);
int dequeue(queue_t *q, pcb_t **p);


#endif // _QUEUE_H
