#include "pqueue.h"

#include <stdlib.h>

/*
#include <stdbool.h>

#include "pcb.h"

#define PQUEUE_MAX_SIZE 1000

typedef struct pqueue pqueue_t;

struct pqueue {
    int size;
    pcb_t *elements[PQUEUE_MAX_SIZE];
};
*/

int pqueue_init(pqueue_t **pq) {
    *pq = malloc(sizeof(pqueue_t));
    (*pq)->size = 0;
    for (int i = 0; i < PQUEUE_MAX_SIZE; i++) (*pq)->elements[i] = NULL;
    return 0;
}

int pqueue_size(pqueue_t *pq) { return pq->size; }

bool pqueue_empty(pqueue_t *pq) { return pq->size == 0; }

int pqueue_front(pqueue_t *pq, pcb_t **p) {
    *p = pq->elements[1];
    return 0;
}

void pqueue_exch(pqueue_t *pq, int i, int j) {
    pcb_t *temp = pq->elements[i];
    pq->elements[i] = pq->elements[j];
    pq->elements[j] = temp;
}

bool pqueue_less(pqueue_t *pq, int i, int j) {
    return pq->elements[i]->priority >= pq->elements[j]->priority;
}

void pqueue_sink(pqueue_t *pq, int k) {
    while (2 * k <= pq->size) {
        int j = 2 * k;
        if (j < pq->size && pqueue_less(pq, j, j+1)) j++;
        if (!pqueue_less(pq, k, j)) break;
        pqueue_exch(pq, k, j);
        k = j;
    }
}

void pqueue_swim(pqueue_t *pq, int k) {
    while (k > 1 && pqueue_less(pq, k/2, k)) {
        pqueue_exch(pq, k, k/2);
        k /= 2;
   }
}

int pqueue_enqueue(pqueue_t *pq, pcb_t *p) {
    pq->elements[++(pq->size)] = p;
    pqueue_swim(pq, pq->size);
    return 0;
}

int pqueue_dequeue(pqueue_t *pq, pcb_t **p) {
    *p = pq->elements[1];
    pqueue_exch(pq, 1, pq->size);
    pq->elements[pq->size--] = NULL;
    pqueue_sink(pq, 1);
    return 0;
}

