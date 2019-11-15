#include "darr.h"

// #include <stdio.h>
#include <stdlib.h>

/*
#define INITIAL_CAPACITY 16

struct darr {
    int len, cap;
    void *arr;
};
*/

int init_darr(struct darr **darr) {
    if (*darr == NULL) {
        if ((*darr = malloc(sizeof(struct darr))) == NULL)
            return -1;    
    }
    
    (*darr)->len = 0;
    (*darr)->cap = INITIAL_CAPACITY;
    if ( ((*darr)->arr = malloc(sizeof(int) * (*darr)->cap)) == NULL )
        return -1; 

    return 0;
}

void free_darr(struct darr *darr) { free(darr->arr); }

int len(struct darr *darr) { return darr->len; }

int cap(struct darr *darr) { return darr->cap; }

int get(struct darr *darr, int idx) {
    return darr->arr[idx];
}

void push_back(struct darr *darr, int x) {
    darr->arr[darr->len++] = x;

    if (darr->len == darr->cap) {
        darr->cap += darr->cap;
        darr->arr = realloc(darr->arr, sizeof(int) * darr->cap);    
    }
}

void pop_back(struct darr *darr, int *x);

/*
int main() {
    struct darr *a;
    init_darr(&a);
    push_back(a, 1);
    push_back(a, 45);
    push_back(a, 2);

    printf("Len %d, Cap %d\n", a->len, a->cap);

    for (int i = 0; i < a->len; i++) printf("a[%d] = %d\n", i, get(a, i));

    return 0;
}
*/

