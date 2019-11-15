#ifndef _BUFFER_H
#define _BUFFER_H


#define BUFFER_CAPACITY 25

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

int  init_buffer(buffer_t **buf);
void free_buffer(buffer_t **buf);
int  get_size(buffer_t *buf);
int  is_empty(buffer_t *buf);
int  is_full(buffer_t *buf);
int  insert_item(buffer_t *buf, buf_item_t it);
int  remove_item(buffer_t *buf, buf_item_t *removed);

// void dump(buffer_t *buf);


#endif // _BUFFER_H
