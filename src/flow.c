/*
	Functions of process messages and logs
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
    Add string to the log file
    Possible types:
    0 - ERROR
    1 - INFO
*/
void wlog(char *log_str, int log_type) {
    FILE *fp;

    char type[8];

    switch(log_type){
        case 0 :
            sprintf(type, "%s", "ERROR");
            break;
        case 1 :
            sprintf(type, "%s", "INFO");
    };

    fp = fopen("hostmon.log", "a");
    fprintf(fp, "%ld %s %s\n", time(NULL), type, log_str);
    fclose(fp);
}

/* Process errors (Prints error message to STDERR and exit */
void err(char *err_str) {
    fprintf(stderr, "Error happened: '%s'\n", err_str);
    wlog(err_str, 0);
    exit(1);
}
