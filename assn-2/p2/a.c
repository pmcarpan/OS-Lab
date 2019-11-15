#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NUM_ITER 10
#define SLEEP_UPPER_LIMIT (4 + 1)

void die(const char *s) {
    perror(s);
    exit(EXIT_FAILURE);
}

void process_loop(const char *info) {
    pid_t curr = getpid();

    // as the parent process state is cloned in child, 
    // the pseudo-random number generator is also in the same state
    // we need to set the random seed of different children to be different
    // only using the current time may not work (time changes insignificantly),
    // so the pid number is also included in the seed
    srand(time(NULL) + 100 * curr);

    for (int i = 0; i < NUM_ITER; i++) {
        printf("P %s / PID %d / Iter %d\n", info, curr, i);
        int sleep_time = 
            (int) ((((double) rand()) / RAND_MAX) * SLEEP_UPPER_LIMIT);
        // printf("Proc %c sleeps for %d sec\n", name, sleep_time);
        sleep( sleep_time );
    }
}

int main(void) {
    pid_t pid_x = fork();

    if (pid_x == -1) die("fork()");

    if (pid_x == 0) {
        process_loop("X");
        kill(getpid(), SIGKILL);
    }

    pid_t pid_y = fork();

    if (pid_y == -1) die("fork()");

    if (pid_y == 0) {
        process_loop("Y");
        kill(getpid(), SIGKILL);
    }

    waitpid(pid_x, NULL, WUNTRACED);
    waitpid(pid_y, NULL, WUNTRACED);

    printf("Parent proc fin\n");


    return EXIT_SUCCESS;
}

