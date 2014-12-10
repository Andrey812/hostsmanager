/*
    Hostman - daemon for monitoring of the LAN hosts
    accordingly hosts's list and monitoring rules.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>
#include <string.h>

#include "../include/flow.h"
#include "../include/cmd.h"
#include "../include/config.h"

/* Display short help and exit */
void show_help()
{
    printf("\nHostman - hosts monitoring and management daemon\n");
    printf("Usage: hostman [-c <config_file>] [-h] [-d]\n");
    printf("\nOptions:\n");
    printf("-c\tPath to the main configuration file with file name\n");
    printf("-d\tEnable debug mode (output debug messages to the log)\n");
    printf("-h\tShow this help and exit\n");
    printf("\nThe same fields of the configuration file have lower priority than command line options.\n");
    exit(0);
}

/*
    Return 1 if application's pid file is locked
    or return 0 if pid file is free
 */
int is_pid_file_locked()
{
    if (!strlen(params.pid_path))
    {
        sprintf(params.pid_path, "hostman.pid");
        sprintf(app.log, "No record about pid path in the configuration. Default path 'hostman.pid' was used.");
        wlog(2,0);
    };

    int pid_file = open(params.pid_path, O_CREAT | O_RDWR, 0666);

    int rc = flock(pid_file, LOCK_EX | LOCK_NB);
    if(rc) {
        if(EWOULDBLOCK == errno)
            return 1;
    }
    else {
            return 0;
    }
};

/* Read and parse command line params */
void read_exec_params(int argc, char **argv)
{
    int arg_i;
    for (arg_i = 0; arg_i < argc; ++arg_i)
    {
        // "-h" - Print short help and exit
        if (!memcmp(argv[arg_i], "-h", 2)) {
            show_help();
        }

        // "-c" - Path to the configuration file
        else if (!memcmp(argv[arg_i], "-c", 2)) {
            if (argv[arg_i+1] == NULL)
            {
                sprintf(app.log, "There is no value for '-c' argument.");
                err();
            }
            sprintf(params.cfg_path, "%s", argv[arg_i+1]);
        }

        // "-d" - If defined - enable debug mode
        // Ddebug flag in the configuration will be overwritten
        else if (!memcmp(argv[arg_i], "-d", 2)) {
            params.debug = 1;
        }
    }
}

void main(int argc, char **argv)
{
    // Read command line arguments
    read_exec_params(argc, argv);

    // Read the main configuration file
    read_cfg();

    // Check if daemon is already executed
    if (is_pid_file_locked())
    {
        sprintf(app.log, "Daemon is already running");
        err();
    }

	// Delete exists info files
	clean_hosts_info();

    // Daemonize process
    pid_t process_id = 0;
    pid_t sid = 0;

    // Create child process
    process_id = fork();

    if (process_id < 0)
    {
        sprintf(app.log, "Fork failed!");
	    err();
    }

    // PARENT PROCESS. Must be killed
    if (process_id > 0)
    {
        sprintf(app.log, "Daemon has started with pid: %d", process_id);
        wlog(1,1);

        exit(0);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Child process - "Daemon" code started from here */

	while(1)
	{
		int host_num = 0;
		while(hosts[host_num].ip != NULL)
		{
            execute_scan_rules(host_num);
            save_host_info(host_num);

			host_num++;
		};
	};
    sprintf(app.log, "Daemon has finished work");
    wlog(1,0);
}


