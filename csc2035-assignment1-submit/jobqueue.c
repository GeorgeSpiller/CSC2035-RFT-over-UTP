/*
 * Replace the following string of 0s with your student number
 * 180250965
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h> // added by student (for printf)
//#include <string.h> // added by student (for memcpy)
#include "jobqueue.h"


const job_t UNUSED_ENTRY = { -1, -1 };

/* 
 * DO NOT EDIT the jq_init function.
 */
void jq_init(jobqueue_t* jq) {
    jq->head = 0;
    jq->tail = 0;
    jq->buf_size = JOBQ_BUF_SIZE;
    
    for (int i = 0; i < jq->buf_size; i++)
        jq->jobs[i] = UNUSED_ENTRY;
}

/* 
 * TODO: you must implement this function to allocate a job queue and 
 * initialise it.
 */
jobqueue_t* jq_new() {
    jobqueue_t *retVal = malloc (sizeof(struct jobqueue)); // struct jobqueue
    if (retVal == NULL) {
		return NULL;	
	}
	jq_init(retVal);
	return retVal;
}

/* 
 * TODO: you must implement this function.
 */
size_t jq_capacity(jobqueue_t* jq) {
	return (size_t)jq->buf_size-1;
}

/* 
 * TODO: you must implement this function.
 * Hints
 * - if the queue is empty, just return an appropriate value for the job
 * - if the queue is not empty, return the value of the entry at the
 *      appropriate position and update queue state before returning.
 *      Remember to mark the queue slot at the position as unused 
 */
job_t jq_dequeue(jobqueue_t* jq) {
		
	if (jq_is_empty(jq)) {
		return UNUSED_ENTRY;
	}

	job_t jq_head_job_original = jq_peekhead(jq);
	jq->jobs[jq->head] = UNUSED_ENTRY;
	jq->head = (jq->head + 1) % jq->buf_size;

	return jq_head_job_original;
}

/* 
 * DO NOT EDIT the jq_enqueue function.
 */
void jq_enqueue(jobqueue_t* jq, job_t j) {  

    if (jq_is_full(jq))
        return;
        
    jq->jobs[jq->tail] = j;

    jq->tail = (jq->tail + 1) % jq->buf_size;
	
}
   
/* 
 * DO NOT EDIT the jq_is_empty function.
 */
bool jq_is_empty(jobqueue_t* jq) {
    return jq->head == jq->tail;
}

/* 
 * TODO: you must implement this function.
 * This function is finished.
 */
bool jq_is_full(jobqueue_t* jq) {
	return jq->head == (jq->tail + 1) % jq->buf_size;
}

/* 
 * DO NOT EDIT the jq_peekhead function.
 */
job_t jq_peekhead(jobqueue_t* jq) {
    return jq->jobs[jq->head];
}

/* 
 * TODO: you must implement this function.
 *
 * Hints
 * - this is not quite as obvious as the peekhead function because the queue
 *      tail value is the position of the next available slot, not the slot
 *      for the value at the end of the queue
 * - remember the buffer wraps round and so the tail could be the position
 *      behind element 0 in the buffer etc
 */
job_t jq_peektail(jobqueue_t* jq) {
	int jq_tail_prev = jq->tail - 1; 
	if (jq_tail_prev == -1) {
		jq_tail_prev = jq->buf_size - 1;
	}
	return jq->jobs[jq_tail_prev];
}

/* 
 * TODO: you must implement this function.
 */
void jq_delete(jobqueue_t* jq) {
	if (jq == NULL) {
		return;
	}
	free(jq);
    return;
}
