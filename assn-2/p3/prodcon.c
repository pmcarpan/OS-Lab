// #include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "buffer.h"

#define PRODUCER_MAX_VAL 80

#define MIN(a,b) ( ((a) > (b)) ? (b) : (a) )

void die(const char *s) {
    perror(s);
    exit(EXIT_FAILURE);
}

// get positive integer in [0, max)
int get_random_integer(int max) {
    return (int) ((((double) rand()) / RAND_MAX) * max);
}

// execution path for producer process
// killed on finish
void producer(int id, buffer_t *buf, sem_t *fill_count_sem, sem_t *empty_count_sem, pthread_mutex_t *buffer_mutex) {
    pid_t curr = getpid();    
    srand(time(NULL) + 100 * curr);

    printf("Prod #%d entry\n", id);    

    // buf_item_t item = {id, id + 1}; 
    buf_item_t item = { id, get_random_integer(PRODUCER_MAX_VAL + 1) };

    sem_wait(empty_count_sem);
        pthread_mutex_lock(buffer_mutex);
            printf("Prod #%d puts %d into buffer\n", id, item.value);
            insert_item(buf, item);
        pthread_mutex_unlock(buffer_mutex);
    sem_post(fill_count_sem);

    printf("Prod #%d exit\n", id);
    kill(curr, SIGKILL);
}

// execution path for consumer process
// killed on finish
void consumer(int id, buffer_t *buf, int *total, int *num_consumed, int max_consumptions, sem_t *fill_count_sem, sem_t *empty_count_sem, sem_t *all_consumed_sem, pthread_mutex_t *buffer_mutex, pthread_mutex_t *total_mutex) {
    pid_t curr = getpid();    
    srand(time(NULL) + 100 * curr);

    printf("Con #%d entry\n", id);

    buf_item_t rem;

    sem_wait(fill_count_sem);               // printf("Con %d entry #1\n", id);
        pthread_mutex_lock(buffer_mutex);   // printf("Con %d entry #2\n", id);
            remove_item(buf, &rem);
            printf("Con #%d got %d from buffer\n", id, rem.value);
        pthread_mutex_unlock(buffer_mutex);
    sem_post(empty_count_sem);

    pthread_mutex_lock(total_mutex);
        *total += rem.value;
        (*num_consumed)++;
        if (*num_consumed == max_consumptions) sem_post(all_consumed_sem);
    pthread_mutex_unlock(total_mutex);

    printf("Con #%d exit\n", id);
    kill(getpid(), SIGKILL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Error, exactly 2 args required.\nFormat is: C-Executable [no. of producers] [no. of consumers]\n");
        exit(EXIT_FAILURE);
    }

    // for shared memory, we use mmap instead of malloc for memory allocations
    // MAP_ANONYMOUS does not require a backing file

    // create #consumed variable
    int *num_consumed = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, 
                             MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (num_consumed == MAP_FAILED) die("mmap()");
    *num_consumed = 0;

    // create total var
    int *total = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, 
                      MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (total == MAP_FAILED) die("mmap()");
    *total = 0;

    // create buffer
    buffer_t *buf = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, 
                         MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (buf == MAP_FAILED) die("mmap()");
    if (init_buffer(&buf) == -1) die("init_buffer()");

    // mutex attributes for sharing among processes
    pthread_mutexattr_t mutexattr; 
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);

    // create total var mutex
    pthread_mutex_t *total_mutex = 
        mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, 
             MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (total_mutex == MAP_FAILED) die("mmap()");
    if (pthread_mutex_init(total_mutex, &mutexattr) != 0) die("pthread_mutex_init()");

    // create buffer mutex
    pthread_mutex_t *buffer_mutex = 
        mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE, 
             MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (buffer_mutex == MAP_FAILED) die("mmap()");
    if (pthread_mutex_init(buffer_mutex, &mutexattr) != 0) die("pthread_mutex_init()");

    // create fill count sem
    sem_t *fill_count_sem = 
        mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, 
             MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (fill_count_sem == MAP_FAILED) die("mmap()");
    if (sem_init(fill_count_sem, 1, 0) == -1) die("sem_init()");

    // create empty count sem
    sem_t *empty_count_sem = 
        mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, 
             MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (empty_count_sem == MAP_FAILED) die("mmap()");
    if (sem_init(empty_count_sem, 1, BUFFER_CAPACITY) == -1) die("sem_init()");

    // create empty count sem
    sem_t *all_consumed_sem = 
        mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, 
             MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (all_consumed_sem == MAP_FAILED) die("mmap()");
    if (sem_init(all_consumed_sem, 1, 0) == -1) die("sem_init()");

    // parse #producers and #consumers from command line args
    int p = (int) strtol(argv[1], NULL, 10),
        c = (int) strtol(argv[2], NULL, 10);

    printf("P = %d, C = %d\n", p, c);

    // create producers
    for (int i = 0; i < p; i++) {
        pid_t pid_prod = fork();
        if (pid_prod == -1) {
            // release the sync tools
            pthread_mutex_destroy(total_mutex); pthread_mutex_destroy(buffer_mutex);
            sem_destroy(fill_count_sem); sem_destroy(empty_count_sem); sem_destroy(all_consumed_sem);

            die("fork()");
        }

        if (pid_prod == 0)
            producer(i, buf, fill_count_sem, empty_count_sem, buffer_mutex);
    }

    int max_consumptions = MIN(p, c);

    // create consumers
    for (int i = 0; i < c; i++) {
        pid_t pid_con = fork();
        if (pid_con == -1) {
            // release the sync tools
            pthread_mutex_destroy(total_mutex); pthread_mutex_destroy(buffer_mutex);
            sem_destroy(fill_count_sem); sem_destroy(empty_count_sem); sem_destroy(all_consumed_sem);

            die("fork()");
        }

        if (pid_con == 0)
            consumer(i, buf, total, num_consumed, max_consumptions, fill_count_sem, empty_count_sem, all_consumed_sem, buffer_mutex, total_mutex);
    }

    sem_wait(all_consumed_sem);
    sleep(1); // allow cleanup of remaining producers and consumers 

    // display the TOTAL var
    printf("TOTAL: %d\n", *total);

    // release the sync tools
    pthread_mutex_destroy(total_mutex); pthread_mutex_destroy(buffer_mutex);
    sem_destroy(fill_count_sem); sem_destroy(empty_count_sem); sem_destroy(all_consumed_sem);

    // explicit unmapping not needed provided the program terminates

    return 0;
}

