/*
	Functions of process messages and logs
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/flow.h"
#include "../include/hosts.h"

/*
    Add string to the log file
    Possible types:
    0 - ERROR
    1 - INFO
    2 - DEBUG (Will be output only when debug mode is enabled)

    to_stdout - if 1 - print this message to stdout (without time and log type)
*/
void wlog(int log_type, int to_stdout) {

    if(log_type == 2 && !params.debug)
        return;

    if(to_stdout == 1)
        printf("%s\n", app.log);

    FILE *fp;

    char type[8];

    switch(log_type){
        case 0 :
            sprintf(type, "%s", "ERROR");
            break;
        case 1 :
            sprintf(type, "%s", "INFO");
            break;
        case 2 :
            sprintf(type, "%s", "DEBUG");
    };

    fp = fopen("hostman.log", "a");
    fprintf(fp, "%ld %s %s\n", time(NULL), type, app.log);
    fclose(fp);

    sprintf(app.log, "%s", "");
}

/* Process errors (Prints error message to STDERR and exit */
void err() {
    wlog(0,1);
    exit(1);
}
