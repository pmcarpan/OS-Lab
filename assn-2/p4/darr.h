#ifndef _DARR_H
#define _DARR_H

#define INITIAL_CAPACITY 8

struct darr {
    int len, cap;
    int *arr;
};

int init_darr(struct darr **darr);
void free_darr(struct darr *darr);
int len(struct darr *darr);
int cap(struct darr *darr);
int get(struct darr *darr, int idx);
void push_back(struct darr *darr, int x);
void pop_back(struct darr *darr, int *x);

#endif
