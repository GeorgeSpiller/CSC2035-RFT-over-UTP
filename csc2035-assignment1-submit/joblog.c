/*
 * Replace the following string of 0s with your student number
 * 180250965
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include "joblog.h"
#include <string.h> // added by student for memcpy

// DO NOT EDIT the following definitions
#define JOBLOG_ENTRY_SIZE 10    // size of the buffer for a log entry <job_id>p<proc_id>'\n''\0'
#define JOBLOG_PATH "./out"     // the parent directory for logs

/* 
 * DO NOT EDIT the new_log_name function. It is a private helper 
 * function provided for you to create a log name from a process 
 * descriptor for use when reading, writing and deleting a log file.
 * 
 * You must work out what the function does in order to use it properly
 * and to clean up after use.
 *
 * See name_fmt below for the form of log names.
 */
static char* new_log_name(proc_t* proc) {
    /* the name_fmt and proctype_label array are declared static so that
     * they are not allocated on every call to the function
     */
    static const char* name_fmt = "%s/%s%02d.txt";
    static const char* proctype_label[] = { 
        "bwait_cons",
        "bwait_prod", 
        "sem_cons", 
        "sem_prod"
    };

    if (!proc)
        return NULL;

    char* log_name;
            
    asprintf(&log_name, name_fmt, JOBLOG_PATH, proctype_label[proc->type],
        proc->id);
        
    return log_name;
}

/* 
 * DO NOT EDIT the jlog_init function.
 */
int jlog_init(proc_t* proc) {
    if (!proc) {
        errno = EINVAL;
        return -1;
    }
        
    int r = 0;
    if (proc->is_init) {
        // create directory if the init process
        struct stat sb;
    
        if (stat(JOBLOG_PATH, &sb) != 0) {
            errno = 0;
            r = mkdir(JOBLOG_PATH, 0777);
        }  else if (!S_ISDIR(sb.st_mode)) {
            unlink(JOBLOG_PATH);
            errno = 0;
            r = mkdir(JOBLOG_PATH, 0777);
        }
    }

    jlog_delete(proc);    // in case log exists for proc
    
    return r;
}

/* 
 * TODO: you must implement this function.
 * Hints:
 * - you will have to define a print format specifier for the log entry,
 *      and this should not be allocated on every function call
 * - remember to append new entries to the log file
 */
void jlog_write_entry(proc_t* proc, job_t* job) {

	static const char* job_fmt = "%05dp%02d\n"; // the job format within the file

	if (proc == NULL || job == NULL) {
		return;
	}

	char* log_file_name = new_log_name(proc);
	int jobid = job->id;
	int pid = job->pid;
	FILE* f = fopen(log_file_name, "a");

    if (!f) {
		errno = 0;
        return;
    }

	fprintf(f, job_fmt, jobid, pid);
	fclose(f);
	
    return;
}

/* 
 * TODO: you must implement this function.
 * Hints:
 * - you have to go to the beginning of the line represented
 *      by entry_num to read the required entry
 * - see malloc and calloc system library functions for dynamic allocation
 * - the space you need to allocate will be larger than the space required 
 *      for the string (constants have been defined to help you)
 */
char* jlog_read_entry(proc_t* proc, int entry_num) {

	char* log_file_name = new_log_name(proc);
	
	if (proc == NULL || log_file_name == NULL || entry_num < 0) { // 
		return NULL;
	}

	FILE* f = fopen(log_file_name, "r");
	int i = 0;
	char buff[JOBLOG_ENTRY_SIZE];
	char *return_buff = (char *) malloc((JOBLOG_ENTRY_SIZE - 1) * sizeof(char));
	
    if (!f) {
		errno = 0;
        return NULL;
    }
	
	while(fgets(buff, JOBLOG_ENTRY_SIZE, f)) {
		if (i == entry_num) {
			fclose(f);
			memcpy(return_buff, &buff, 8);
			return_buff[(JOBLOG_ENTRY_SIZE - 2)] = '\0';
			return return_buff;
		}
		i++;
    }
	fclose(f);
	errno = 0;
    return NULL;
}

/* 3
 * TODO: you must implement this function.
 */
void jlog_delete(proc_t* proc) {
	char* log_file_name = new_log_name(proc);
	if (proc == NULL || log_file_name == NULL) {
		return;
	}

	remove(log_file_name);
	errno = 0;
    return;
}