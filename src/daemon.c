/*
    hostmon - daemon for monitoring of the LAN hosts
    according hosts's list and monitoring rules.
    Result about each host will be placed to the *.info file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>

#include "../include/cmd.h"
#include "../include/flow.h"
#include "../include/hosts.h"

#include "../include/raidix.h"

int is_pid_file_exists()
{
    int pid_file = open("hostmon.pid", O_CREAT | O_RDWR, 0666);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);
    if(rc) {
        if(EWOULDBLOCK == errno)
            return 1;
    }
    else {
            return 0;
    }
};

void main()
{
    if (is_pid_file_exists())
        err("Daemon is already running");

    // Get info about hosts from configuration file
    read_cfg();

	// Delete exists info files
	clean_hosts_info();

    /* Daemonize process */
    pid_t process_id = 0;
    pid_t sid = 0;

    // Create child process
    process_id = fork();

    if (process_id < 0)
		err("Fork failed!");

    // PARENT PROCESS. Must be killed
    if (process_id > 0)
    {
        char started_msg[32];
        sprintf(started_msg, "Daemon has started with pid: %d", process_id);
        wlog(started_msg, 1);

        printf("Daemon has started (pid: %d)\n", process_id);
        exit(0);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Child process - "Daemon" code started from here */

	while(1)
	{
		int host_num = 0;
		while(hosts[host_num].cfg_name != NULL)
		{
			// Ping
			hosts[host_num].ping = ping(hosts[host_num].ip);

			// Check version
			ver(hosts[host_num].ip, host_num);

			// Save result about host to the file
			save_host_info(host_num);

			host_num++;
		};
	};

    wlog("Daemon has finished work", 1);
}


