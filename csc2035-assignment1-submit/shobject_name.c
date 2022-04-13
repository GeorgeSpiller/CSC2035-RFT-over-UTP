/*
 * Replace the following string of 0s with your student number
 * 180250965
 */
#include <stdlib.h>
#include <stdio.h>
#include "shobject_name.h"

/* 
 * TODO you must implement this function.
 *
 * Hints:
 * - from its specification in shobject_name.h this function may seem 
 *      complicated, in fact it is very simple and almost everything is 
 *      covered in C programming module materials
 * - there is a library function for getting the current user from the 
 *      environment (use "anon" if the function returns NULL)
 */
void shobject_name(char* name_buf, const char* label) {

	char* username = getenv("USER");
	
	if (!username) {
		username = "anon";
	} else if (username[0] == '\0' || !username[0]) {
		username = "none";
	}
	
	if (!label || !label[0]) {
		label = "none";
	}
		
	snprintf(name_buf, MAX_SHOBJ_NAME_SIZE, SHOBJ_NAME_FORMAT, username, label);
	
    return;


}
