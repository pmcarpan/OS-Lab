#ifndef _PCB_H
#define _PCB_H


#include <stdbool.h>
#include <stdio.h>

typedef struct pcb pcb_t;

// TODO: bursts array may be changed to linked list or dynamic array

struct pcb {
    int job_id, priority, arr_time, wait_time;
    bool cpu_burst;
    int curr_cpu_burst_idx, curr_io_burst_idx;
    int cpu_bursts[100], io_bursts[100];
};

int read_pcb_from_file(FILE *fp, pcb_t **pcb);


#endif // _PCB_H
