#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
        if (q == NULL) return 1;
	return (q->size == 0);
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
        /* TODO: put a new process to queue [q] */
        if ( q != NULL && proc != NULL && q->size < MAX_QUEUE_SIZE )
        {
                q->proc[q->size++] = proc;
        }
        
}

struct pcb_t * dequeue(struct queue_t * q) {
        /* TODO: return a pcb whose prioprity is the highest
         * in the queue [q] and remember to remove it from q
         * */
	if (q == NULL || q->size == 0 ) return NULL;
        struct pcb_t *prio_proc = NULL;
        unsigned long highest_prio = MAX_PRIO; // use the value out of range to check the highest_prior 
        int highest_num = -1; // avoid call 0 -> max_prio 
        for (int i = 0 ; i < q->size ; i ++)
        {
                if (q->proc[i] != NULL && q->proc[i]->prio < highest_prio)
                {
                        highest_prio = q->proc[i]->prio;
                        highest_num = i;
                }
        }
        prio_proc = q->proc[highest_num];
        q->size--;
        for (int i = highest_num; i < q->size ; i ++)
        {
                q->proc[i] = q->proc[i+1];
        }
        q->proc[q->size] = NULL;
        return prio_proc;
}