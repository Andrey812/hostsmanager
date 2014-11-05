/*
	Functions of process messages and logs
*/

#include <stdio.h>
#include <stdlib.h>

/* Process errors (Prints error message to STDERR and exit */
void err(char *err_str) {
    fprintf(stderr, "Error happened: '%s'\n", err_str);
    exit(0);
}

