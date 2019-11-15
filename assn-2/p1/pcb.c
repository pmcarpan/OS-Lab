#include "pcb.h"

#include <stdbool.h>
#include <stdlib.h>

/*
typedef struct pcb pcb_t;

// TODO: bursts array may be changed to linked list or dynamic array

struct pcb {
    int job_id, priority, arr_time, wait_time;
    int curr_cpu_burst_idx, curr_io_burst_idx;
    int cpu_bursts[100], io_bursts[100];
};
*/

int read_pcb_from_file(FILE *fp, pcb_t **pcb) {
    *pcb = malloc(sizeof(pcb_t));
    (*pcb)->wait_time = 0; 
    (*pcb)->curr_cpu_burst_idx = 0; (*pcb)->curr_io_burst_idx = 0;

    int arr[3];
    int matched = fscanf(fp, "%d %d %d", &arr[0], &arr[1], &arr[2]);
    if (matched != 3) return -1;

    (*pcb)->job_id = arr[0];
    (*pcb)->priority = arr[1];
    (*pcb)->arr_time = arr[2];

    int scanned_value, i = 0, j = 0; bool cpu_burst = true;
    do {
        fscanf(fp, "%d", &scanned_value); if (scanned_value == -1) break;
        if (cpu_burst) {
            (*pcb)->cpu_bursts[i++] = scanned_value;
        }
        else {
            (*pcb)->io_bursts[j++] = scanned_value;
        }
        cpu_burst = !cpu_burst;
    } while (scanned_value != -1);

    (*pcb)->cpu_bursts[i++] = -1;
    (*pcb)->io_bursts[j++] = -1;

    return 0;
}

