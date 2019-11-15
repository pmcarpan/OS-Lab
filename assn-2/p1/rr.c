#include <stdio.h>
#include <stdlib.h>

#include "pcb.h"
#include "queue.h"

#define MAX_QUANTUMS 5
#define CLOCKS_UPPER_LIMIT 10000

static int clocks = 0,
           quantums = 0;
static int num_proc = 0,
           tot_wait_time = 0,
           tot_turn_time = 0;
static queue_t *sending_queue = NULL, 
               *ready_queue   = NULL, 
               *io_queue      = NULL;

void init(const char *filename) {
    // read from file and add pcbs to sending queue
    FILE *fp = fopen(filename, "r");
    init_queue(&sending_queue);

    pcb_t *tmp;
    while (read_pcb_from_file(fp, &tmp) != -1) {
        //printf("Job %d Pr %d Arr Time %d\n", tmp->job_id, tmp->priority, tmp->arr_time);
        //for (int i = 0; tmp->cpu_bursts[i] != -1; i++) printf("%d ", tmp->cpu_bursts[i]);
        //printf("\n");
        enqueue(sending_queue, tmp);
    }

    fclose(fp);

    // initialize ready and io queues
    init_queue(&ready_queue);
    init_queue(&io_queue);
}

void disp(pcb_t *pcb) {
    printf("Id %d CBIdx %d (%d) IBIdx %d (%d)\n", pcb->job_id, pcb->curr_cpu_burst_idx, 
           pcb->cpu_bursts[pcb->curr_cpu_burst_idx], pcb->curr_io_burst_idx, pcb->io_bursts[pcb->curr_io_burst_idx]);
}

void proc() {
    printf("Size S %d R %d I %d\n", size(sending_queue), size(ready_queue), size(io_queue));

    pcb_t *temp_pcb = NULL; queue_node_t *curr = NULL;
    pcb_t *put_in_io_queue = NULL, *put_in_ready_queue = NULL;

    // while ( sending queue, ready queue and io queues are non-empty ) {
    // move from sending queue to ready queue
    while (!empty(sending_queue)) {
        front(sending_queue, &temp_pcb);
        if (temp_pcb->arr_time > clocks) break;
        dequeue(sending_queue, &temp_pcb);

        temp_pcb->cpu_burst = true; temp_pcb->wait_time = 0;
 
        enqueue(ready_queue, temp_pcb); 
    }

    // increase waiting times
    if (!empty(ready_queue)) {
        curr = ready_queue->head->next;
        while (curr) { curr->pcb->wait_time++; curr = curr->next; }
    }

    // process ready queue first item
    // if finished, increase turnaround time and processes counter
    if (empty(ready_queue)) goto io_queue;
    front(ready_queue, &temp_pcb); // disp(temp_pcb);
    temp_pcb->cpu_bursts[temp_pcb->curr_cpu_burst_idx]--;
    quantums++;
    if (temp_pcb->cpu_bursts[temp_pcb->curr_cpu_burst_idx] == 0) {
        temp_pcb->curr_cpu_burst_idx++;
        
        if (temp_pcb->cpu_bursts[temp_pcb->curr_cpu_burst_idx] == -1) {
            dequeue(ready_queue, &temp_pcb);

            tot_wait_time += temp_pcb->wait_time;
            tot_turn_time += clocks - temp_pcb->arr_time + 1;

            free(temp_pcb);
            num_proc++;
        }
        else {
            dequeue(ready_queue, &put_in_io_queue);
            // enqueue(io_queue, temp_pcb);
        }
        quantums = 0;
    }
    else if (quantums >= MAX_QUANTUMS && size(ready_queue) > 1) {
        dequeue(ready_queue, &temp_pcb);
        enqueue(ready_queue, temp_pcb);            
        quantums = 0;
    }

io_queue:
    // process io queue first item
    if (empty(io_queue)) goto clock_increment;    
    front(io_queue, &temp_pcb);
    temp_pcb->io_bursts[temp_pcb->curr_io_burst_idx]--;
    if (temp_pcb->io_bursts[temp_pcb->curr_io_burst_idx] == 0) {
        temp_pcb->curr_io_burst_idx++;

        dequeue(io_queue, &put_in_ready_queue);
        // enqueue(ready_queue, temp_pcb);
    }
    
clock_increment:
    if (put_in_io_queue) { enqueue(io_queue, put_in_io_queue); put_in_io_queue = NULL; }
    if (put_in_ready_queue) { enqueue(ready_queue, put_in_ready_queue); put_in_ready_queue = NULL; }
    // inc clock
    clocks++;
}

int main(int argc, char *argv[]) {
    if (argc != 2) { printf("Format: [C-Executable] [filename]\n"); return EXIT_FAILURE; }

    init(argv[1]);
    while (clocks < CLOCKS_UPPER_LIMIT && !(empty(sending_queue) && empty(ready_queue) && empty(io_queue)))    
        proc();

    printf("Tot Processes: %d\n", num_proc);
    printf("Waiting Time   (Tot / Avg): %10d / %10.2f\n", tot_wait_time, (double) tot_wait_time / num_proc);
    printf("Turaround Time (Tot / Avg): %10d / %10.2f\n", tot_turn_time, (double) tot_turn_time / num_proc);

    return EXIT_SUCCESS;
}

