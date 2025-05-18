/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */
#include"stdlib.h"
#include "common.h"
#include "syscall.h"
#include "stdio.h"
#include "libmem.h"
#include "string.h"
#include "queue.h"

void remove_from_queue(struct queue_t *q, int idx) {
    for (int i = idx; i < q->size - 1; i++) {
        q->proc[i] = q->proc[i + 1];
    }
    q->size--;
    q->proc[q->size] = NULL;
}


int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
{
    char proc_name[100];
    uint32_t data;

    //hardcode for demo only
    uint32_t memrg = regs->a1;
    
    /* TODO: Get name of the target proc */
    //proc_name = libread..
    int i = 0;
    data = 0;
    while(data != -1){
        libread(caller, memrg, i, &data);
        proc_name[i]= data;
        if(data == -1) proc_name[i]='\0';
        i++;
        if (caller->code->size < i) { proc_name[i] = '\0'; break; }
    }  
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    /* TODO: Traverse proclist to terminate the proc
     *       stcmp to check the process match proc_name
     */
    //caller->running_list
    //caller->mlq_ready_queu
    struct queue_t *queue = caller->mlq_ready_queue;
    struct queue_t *rlist = caller->running_list;

    /* TODO Maching and terminating 
     *       all processes with given
     *        name in var proc_name
     */  
    for (int idx = 0; idx < queue->size; idx++) {
        struct pcb_t *p = queue->proc[idx];
        char name[100];
        sprintf(name, "P%d", p->pid);
        if (strcmp(name, proc_name) == 0) {
            printf("the remaining process : %d\n",queue->size);
            if (p->mm != NULL) { 
                libfree(p, memrg);
                free_pcb_memph(p);
            }
            printf("[Killall] Terminating ready PID=%d (%s)\n", p->pid, p->path);
            remove_from_queue(queue, idx);
            idx--; 
            printf("the remaining process : %d\n",queue->size);
        }
    }

    for (int idx = 0; idx < rlist->size; idx++) {
        struct pcb_t *p = rlist->proc[idx];
        char name[100];
        sprintf(name, "P%d", p->pid);
        if (strcmp(name, proc_name) == 0) {
            printf("the remaining process : %d\n",rlist->size);
            if (p->mm != NULL) { 
                libfree(p, memrg);
                free_pcb_memph(p);
            }
            printf("[Killall] Terminating running PID=%d (%s)\n", p->pid, p->path);
            remove_from_queue(rlist, idx);
            idx--;
            printf("the remaining process : %d\n",rlist->size);
        }
    }
    return 0; 
}
