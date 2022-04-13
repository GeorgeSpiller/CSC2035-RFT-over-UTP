	/*
 * Replace the following string of 0s with your student number
 * 180250965
 */
#include "mutex_peterson.h"
#include <unistd.h> // added by student, used for sleep() for busy wait

/* mutex.h contains the specification of functions in this file */

/* 
 * TODO: you must implement this function
 * Hints:
 * - see mutex_lockvar.c for how mu_new is implemented there but remember
 *      that the underlying mutex type allocated in shared memory and 
 *      encapsulated by the IPC type will be different
 * - the shared object label should be mux_peters
 * - also see: mutex_peterson.h, mutex_lockvar.c and ipc.h
 */
mutex_t* mu_new(proc_t* proc) {

    ipc_t* ipc_new_peterson = ipc_new(proc, "mux_peters", sizeof(mutex_peterson_t));

	return ipc_new_peterson;

}

/* 
 * TODO: you must implement this function.
 * Hints:
 * - see the section on the Peterson's busy waiting solution in 
 *     the operating systems material on IPC
 * - process ids must be normalised to either a 0 or 1 for indexing 
 *      into the interested array (there is an arithmetic operator to do this)
 * - remember a mutex_t object is basically an ipc object and the specific
 *      implementation of a mutex (Peterson's in this case) in
 *      shared memory is at the addr field (see ipc.h)
 * - also see: mutex_lockvar.c
 */
void mu_enter(mutex_t* mux) {

	if (!mux) {return;}
	
	mutex_peterson_t* mutex_peterson_struct = (mutex_peterson_t*) mux->addr;
	int this_id = mux->proc->id;
	int other_id = (1-this_id);
	
	mutex_peterson_struct->interested[this_id % 2] = 1; 
	mutex_peterson_struct->turn = this_id;
	
	while (mutex_peterson_struct->turn == this_id && mutex_peterson_struct->interested[other_id % 2] == 1) {
		sleep(1);
	}

}

/* 
 * TODO: you must implement this function.
 * Hints: see hints for mu_enter
 */
void mu_leave(mutex_t* mux) {

	if (!mux) {return;}
	
	mutex_peterson_t* mutex_peterson_struct = (mutex_peterson_t*) mux->addr;
	int this_id = mux->proc->id;
	
	mutex_peterson_struct->interested[this_id % 2] = 0; 

    return;
}

/* 
 * TODO: you must implement this function.
 * Hint:
 * - deallocate what you allocate in mu_new
 */
void mu_delete(mutex_t* mux) {
	free(mux);

   return;
}
