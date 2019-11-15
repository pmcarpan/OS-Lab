#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
// #include <time.h>
#include <unistd.h>

#include "darr.h"

// TODO: try to map actual file pointer ?
// #define SHARED_FILE_NAME "shr.txt"

#define SHARED_BUFFER_SIZE 4096
#define BUFFER_FLUSH_LIMIT 64

// declare the shared resources
static char *buffer = NULL;
static int *read_count = NULL, *write_count = NULL; 
static sem_t *rmutex = NULL, *wmutex = NULL, *read_try = NULL, 
             *resource = NULL;

void die(const char *s) {
    perror(s);
    exit(EXIT_FAILURE);
}

// get positive integer in [0, max)
int get_random_integer(int max) {
    return (int) ((((double) rand()) / RAND_MAX) * max);
}

void reader(int id) {
    printf("> Reader %d enters\n", id); fflush(stdout);

    // <ENTRY Section>
    sem_wait(read_try);                 // a reader is trying to enter
        sem_wait(rmutex);               // lock entry section to avoid race condition with other readers
            (*read_count)++;            // report yourself as a reader
            if (*read_count == 1)       // checks if you are first reader
                sem_wait(resource);     // if you are first reader, lock  the resource
        sem_post(rmutex);               // release entry section for other readers
    sem_post(read_try);                 // indicate you are done trying to access the resource

    // <CRITICAL Section>
    // reading is performed
    printf("    Reader %d reads; buf: %s\n", id, buffer); fflush(stdout);

    // <EXIT Section>
    sem_wait(rmutex);                   // reserve exit section - avoids race condition with readers
        (*read_count)--;                // indicate you're leaving
        if (*read_count == 0)           // checks if you are last reader leaving
            sem_post(resource);         // if last, you must release the locked resource
    sem_post(rmutex);                   // release exit section for other readers

    printf("< Reader %d exits\n", id); fflush(stdout);

    kill(getpid(), SIGKILL);
}

void writer(int id) {
    printf("> Writer %d enters\n", id); fflush(stdout);

    // <ENTRY Section>
    sem_wait(wmutex);                   // reserve entry section for writers - avoids race conditions
        (*write_count)++;               // report yourself as a writer entering
        if (*write_count == 1)          // checks if you're first writer
            sem_wait(read_try);         // if you're first, then you must lock the readers out. Prevent them from trying to enter CS
    sem_post(wmutex);                   // release entry section

    // <CRITICAL Section>
    sem_wait(resource);                 // reserve the resource for yourself - prevents other writers from simultaneously editing the shared resource
        // writing is performed

        printf("    Writer %d writes\n", id); fflush(stdout);
        char temp[32];
        sprintf(temp, " | W %3d |", id);
        // strcat(buffer, temp); // this is MT-safe acc to POSIX

        // here we simulate a thread unsafe write operation
        // using single character writes and sleeping for 0 to 0.5 ms inbetween
        int len1 = strlen(buffer), len2 = strlen(temp);
        for (int i = len1; i < len1 + len2; i++) { 
            buffer[i] = temp[i - len1]; usleep(get_random_integer(5 + 1) * 100); 
        }
        buffer[len1 + len2] = '\0';
    sem_post(resource);                 // release file

    // <EXIT Section>
    sem_wait(wmutex);                   // reserve exit section
        (*write_count)--;               // indicate you're leaving
        if (*write_count == 0)          // checks if you're the last writer
            sem_post(read_try);         // if you're last writer, you must unlock the readers. Allows them to try enter CS for reading
    sem_post(wmutex); 

    printf("< Writer %d exits\n", id); fflush(stdout);

    kill(getpid(), SIGKILL);
}

void shared_mem_alloc() {
    buffer = mmap(NULL, sizeof(char) * SHARED_BUFFER_SIZE, PROT_READ | PROT_WRITE, 
                           MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (buffer == MAP_FAILED) die("mmap()");
    buffer[0] = '\0';

    read_count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, 
                           MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (read_count == MAP_FAILED) die("mmap()");
    *read_count = 0;

    write_count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, 
                           MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (write_count == MAP_FAILED) die("mmap()");
    *write_count = 0;

    rmutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, 
                       MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (rmutex == MAP_FAILED) die("mmap()");
    if (sem_init(rmutex, 1, 1) == -1) die("sem_init()");

    wmutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, 
                       MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (wmutex == MAP_FAILED) die("mmap()");
    if (sem_init(wmutex, 1, 1) == -1) die("sem_init()");

    read_try = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, 
                       MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (read_try == MAP_FAILED) die("mmap()");
    if (sem_init(read_try, 1, 1) == -1) die("sem_init()");

    resource = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, 
                       MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (resource == MAP_FAILED) die("mmap()");
    if (sem_init(resource, 1, 1) == -1) die("sem_init()");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Error, exactly 2 args required.\nFormat is: C-Executable [no. of readers] [no. of writers]\n");
        exit(EXIT_FAILURE);
    }    

    shared_mem_alloc();

    // parse #readers and #writers from command line args
    int r = (int) strtol(argv[1], NULL, 10),
        w = (int) strtol(argv[2], NULL, 10);

    printf("R = %d, W = %d\n", r, w); fflush(stdout);

    struct darr *arr = NULL; 
    init_darr(&arr);

    for (int i = 0; i < r; i++) {
        pid_t pid_reader = fork();
        if (pid_reader == -1)   die("fork()");

        if (pid_reader == 0)    reader(i);
        else                    push_back(arr, (int) pid_reader);   
    }

    for (int i = 0; i < w; i++) {
        pid_t pid_writer = fork();
        if (pid_writer == -1)   die("fork()");

        if (pid_writer == 0)    writer(i);
        else                    push_back(arr, (int) pid_writer);   
    }

    // wait on all the child processes
    for (int i = 0; i < len(arr); i++) 
        waitpid((pid_t) get(arr, i), NULL, WUNTRACED);
    // sleep(1);

    printf("Final buffer: %s\n", buffer); 

    int limit = BUFFER_FLUSH_LIMIT;
    printf("First %d chars of buffer:\n", limit);
    for (int i = 0; i < limit; i++) printf("%c", buffer[i] ? buffer[i] : 'n');
    printf("\n"); fflush(stdout);

    return EXIT_SUCCESS;
}

