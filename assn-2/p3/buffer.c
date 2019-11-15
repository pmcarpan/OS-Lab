#include "buffer.h"

// #include <stdio.h>
#include <stdlib.h>

/*
#define BUFFER_CAPACITY 5
 
typedef struct buf_item buf_item_t;
typedef struct buffer buffer_t;

struct buf_item {
    int id;
    int value;
};

struct buffer {
    buf_item_t items[BUFFER_CAPACITY];
    int start, end, size;
};
*/

int init_buffer(buffer_t **buf) {
    if (*buf == NULL) {
        *buf = (buffer_t *) malloc(sizeof(buffer_t));
        if (*buf == NULL) return -1;
    }

    (*buf)->start = 0;
    (*buf)->end = 0;
    (*buf)->size = 0;
    return 0;
}
void free_buffer(buffer_t **buf) {
    free(*buf);
    *buf = NULL;
}

int get_size(buffer_t *buf) { return buf->size; }

int is_empty(buffer_t *buf) { return buf->size == 0; }

int is_full(buffer_t *buf) { return buf->size == BUFFER_CAPACITY; }

int insert_item(buffer_t *buf, buf_item_t it) {
    if (is_full(buf)) return -1;

    if (is_empty(buf)) { buf->start = 0; buf->end = 0; }
    buf->items[buf->end] = it;
    buf->end = (buf->end + 1) % BUFFER_CAPACITY;
    buf->size++;
    return 0;
}

int remove_item(buffer_t *buf, buf_item_t *removed) {
    if (is_empty(buf)) return -1;

    if (removed) *removed = buf->items[buf->start];
    buf->start = (buf->start + 1) % BUFFER_CAPACITY;
    buf->size--;
    return 0;
}

/*
void dump(buffer_t *b) {
    printf("size: %d, start: %d, end: %d\n", b->size, b->start, b->end);
    for (int i = 0; i < b->size; i++) {
        int curr = (b->start + i) % BUFFER_CAPACITY;
        printf("(%d, %d)\n", b->items[curr].id, b->items[curr].value);    
    }
}
*/

