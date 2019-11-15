#ifndef _PQUEUE_H
#define _PQUEUE_H

#include <stdbool.h>

#include "pcb.h"

#define PQUEUE_MAX_SIZE 1000

typedef struct pqueue pqueue_t;

struct pqueue {
    int size;
    pcb_t *elements[PQUEUE_MAX_SIZE];
};

int  pqueue_init(pqueue_t **pq);
int  pqueue_size(pqueue_t *pq);
bool pqueue_empty(pqueue_t *pq);
int  pqueue_front(pqueue_t *pq, pcb_t **p);
void pqueue_exch(pqueue_t *pq, int i, int j);
bool pqueue_less(pqueue_t *pq, int i, int j);
void pqueue_sink(pqueue_t *pq, int k);
void pqueue_swim(pqueue_t *pq, int k);
int  pqueue_enqueue(pqueue_t *pq, pcb_t *p);
int  pqueue_dequeue(pqueue_t *pq, pcb_t **p);


#endif // _PQUEUE_H
