#include "queue.h"

#include <stdlib.h>

/*
typedef struct queue_node queue_node_t;
typedef struct queue queue_t;

struct queue_node {
    pcb_t *pcb;
    queue_node_t next;
};

struct queue {
    int size;
    queue_node_t *head, *tail;
};
*/

int init_queue(queue_t **q) {
    *q = malloc(sizeof(queue_t));
    (*q)->size = 0;
    (*q)->head = NULL; 
    (*q)->tail = NULL;
    return 0;
}

int size(queue_t *q) { return q->size; }

bool empty(queue_t *q) { return q->size == 0; }

int front(queue_t *q, pcb_t **p) {
    *p = q->head->pcb;    
    return 0;
}

int enqueue(queue_t *q, pcb_t *p) {
    queue_node_t *temp = malloc(sizeof(queue_node_t));
    if (temp == NULL) return -1;

    temp->pcb = p; 
    temp->next = NULL;

    if (q->head == NULL) q->head = q->tail = temp;
    else {
        q->tail->next = temp;
        q->tail = q->tail->next;
    }
    q->size++;
    return 0;
}

int dequeue(queue_t *q, pcb_t **p) {
    if (q->size == 0) return -1;

    *p = q->head->pcb;
    queue_node_t *temp = q->head;
    q->head = q->head->next;
    free(temp);

    q->size--;
    if (q->size == 0) q->tail = NULL;
    return 0;
}

// int main() { return 0; }

