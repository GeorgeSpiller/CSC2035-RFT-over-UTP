/*
 * Replace the following string of 0s with your student number
 * 180250965
 */
#include <fcntl.h>          /* For O_* constants */
#include <sys/stat.h>       /* For mode constants */
#include <semaphore.h>
#include "sem_ipc_jobqueue.h"

/* 
 * DO NOT EDIT the following declarations that are used to detect
 * failures in semaphore setup
 */
#define SEM_NEW_FAIL        000
#define MUTEX_SEM_SUCCESS   001
#define FULL_SEM_SUCCESS    002
#define EMPTY_SEM_SUCCESS   004
#define ALL_SEM_SUCCESS     007

/* 
 * DO NOT EDIT the following semaphore names.
 */
static const char* sem_mutex_label = "sijq.mutex";
static const char* sem_full_label = "sijq.full";
static const char* sem_empty_label = "sijq.empty";


/* 
 * DO NOT EDIT the private helper function sem_new for creating new
 * semaphores at initialisation
 */
static int sem_new(sem_t** sem, const char* sem_label, int init_value, 
    int success) {
    char sem_name[MAX_SHOBJ_NAME_SIZE];
    
    shobject_name(sem_name, sem_label);
    
    sem_t* new_sem = sem_open(sem_name, O_CREAT, S_IRWXU, init_value);
    
    if (new_sem == SEM_FAILED)
        return SEM_NEW_FAIL;
    
    *sem = new_sem;
        
    return success;
}

/* 
 * DO NOT EDIT the private helper function sem_delete for closing semaphores
 * when a sem_ipc_jqueue_t is deleted.
 */
static void sem_delete(sem_t* sem, const char* sem_label) {
    char sem_name[MAX_SHOBJ_NAME_SIZE];
    sem_close(sem);
    shobject_name(sem_name, sem_label);
    sem_unlink(sem_name);
}

/* 
 * DO NOT EDIT the sem_ijq_new that creates a new sem_ipc_jobqueue_t
 * and associated semaphores.
 * You will need to look at this function to see what needs to be deleted, 
 * freed or closed by sem_ijq_new
 */
sem_ipc_jobqueue_t* sem_ijq_new(proc_t* proc) {
    sem_ipc_jobqueue_t* sijq = (sem_ipc_jobqueue_t*)
                                    malloc(sizeof(sem_ipc_jobqueue_t));

    if (!sijq)
        return NULL;
        
    sijq->ijq = ipc_jq_new(proc);   // delays all but init process
    
    if (!sijq->ijq) {
        free(sijq);
        return NULL;
    }
    
    int r = sem_new(&sijq->mutex, sem_mutex_label, 1, MUTEX_SEM_SUCCESS);
    
    if (r != MUTEX_SEM_SUCCESS) {
        ipc_jq_delete(sijq->ijq);
        free(sijq);
        return NULL;
    }
    
    sem_wait(sijq->mutex);
    
    r |= sem_new(&sijq->full, sem_full_label, 0, FULL_SEM_SUCCESS)
            | sem_new(&sijq->empty, sem_empty_label, ipc_jq_capacity(sijq->ijq),
                EMPTY_SEM_SUCCESS);
    
    if (r & ALL_SEM_SUCCESS) {
        sem_post(sijq->mutex);
        return sijq;    // all succeeded
    }
    
    // mutex failures    
    if (r & FULL_SEM_SUCCESS)
        sem_delete(sijq->full, sem_full_label);

    if (r & EMPTY_SEM_SUCCESS)
        sem_delete(sijq->empty, sem_empty_label);

    sem_post(sijq->mutex);
    sem_delete(sijq->mutex, sem_mutex_label);
    ipc_jq_delete(sijq->ijq);
    free(sijq);
                
    return NULL;
}

/* 
 * TODO: you must implement this function according to the specification in
 * sem_ipc_jobqueue.h
 */
size_t sem_ijq_capacity(sem_ipc_jobqueue_t* c) {
	size_t return_val = 0;
	if (sem_wait(c->mutex) == 0) {
		return_val = ipc_jq_capacity(c->ijq);
	}
	sem_post(c->mutex);
	
    return return_val;
}

/* 
 * TODO: you must implement this function according to the specification in
 * sem_ipc_jobqueue.h
 */
job_t sem_ijq_dequeue(sem_ipc_jobqueue_t* sijq) {
	
	job_t return_val = UNUSED_ENTRY;

	if (sem_wait(sijq->full) == 0) {
		if (sem_wait(sijq->mutex) == 0) {
			return_val = ipc_jq_dequeue(sijq->ijq);
		}
		sem_post(sijq->mutex);
	}
	sem_post(sijq->empty);

    return return_val;
}

/* 
 * TODO: you must implement this function according to the specification in
 * sem_ipc_jobqueue.h
 */
void sem_ijq_enqueue(sem_ipc_jobqueue_t* sijq, job_t j) {

	if (sem_wait(sijq->empty) == 0) {
		if (sem_wait(sijq->mutex) == 0) {
			ipc_jq_enqueue(sijq->ijq, j);
		}
		sem_post(sijq->mutex);
	}
	sem_post(sijq->full);
    return;
}       

/* 
 * TODO: you must implement this function according to the specification in
 * sem_ipc_jobqueue.h
 */
bool sem_ijq_is_empty(sem_ipc_jobqueue_t* sijq) {
	bool return_val = false;
	int sem_wait_flag = 0;
	
	sem_wait_flag = sem_wait(sijq->mutex);
	if (sem_wait_flag != 0) {
		sem_post(sijq->mutex);
		return return_val;
	}
	
	return_val = ipc_jq_is_empty(sijq->ijq);
	
	sem_post(sijq->mutex);

    return return_val;
}

/* 
 * TODO: you must implement this function according to the specification in
 * sem_ipc_jobqueue.h
 */
bool sem_ijq_is_full(sem_ipc_jobqueue_t* sijq) {
	bool return_val = false;
	
	if (sem_wait(sijq->mutex) == 0) {
		return_val = ipc_jq_is_full(sijq->ijq);
		sem_post(sijq->mutex);
		return return_val;
	} else {
		sem_post(sijq->mutex);
		return return_val;
	}

}

/* 
 * TODO: you must implement this function according to the specification in
 * sem_ipc_jobqueue.h
 */
job_t sem_ijq_peekhead(sem_ipc_jobqueue_t* sijq) {
	
	job_t return_val = UNUSED_ENTRY;

	if (sem_wait(sijq->mutex) == 0) {
		return_val = ipc_jq_peekhead(sijq->ijq);
	}
	sem_post(sijq->mutex);
	
    return return_val;

}

/* 
 * TODO: you must implement this function according to the specification in
 * sem_ipc_jobqueue.h
 */
job_t sem_ijq_peektail(sem_ipc_jobqueue_t* sijq) {
	
    job_t return_val = UNUSED_ENTRY;

	if (sem_wait(sijq->mutex) == 0) {
		return_val = ipc_jq_peektail(sijq->ijq);
	}
	sem_post(sijq->mutex);
	
    return return_val;
}

/* 
 * TODO: you must implement this function according to the specification in
 * sem_ipc_jobqueue.h
 * Hint:
 * - look at what is allocated and/or opened in sem_ijq_new
 */
void sem_ijq_delete(sem_ipc_jobqueue_t* sijq) {
	sem_destroy(sijq->mutex);
	sem_destroy(sijq->full);
	sem_destroy(sijq->empty);
	
	ipc_jq_delete(sijq->ijq);
	free(sijq);
       
    return;
}
 