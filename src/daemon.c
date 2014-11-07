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

/*
    Process ping and extract result
    Returns:
    0 - host is unavailable
    1 - host is available
*/
int ping(char *host) {
    int reg_ret, str_num, reg_err;

    // Build ping command string with attributes
    char *cmd_tpl = "ping {0} -c 2";
    char *cmd_attrs[] = {host};
    char cmd[256];

    make_cmd(cmd_tpl, cmd_attrs, cmd);

    // Execute ping command and get answer
    char *ping_answ[32];
    int ping_answ_str_cnt = 0;

    exec_cmd(cmd, ping_answ, &ping_answ_str_cnt);

    // Parse ping answer
    regex_t regex; //regex for parsing of the ping answer
    reg_err = regcomp(&regex, "time=", 0);
    if (reg_err)
        wlog("Could not compile regex", 1);

    for (str_num = 0; str_num < ping_answ_str_cnt; str_num++)
    {
        reg_ret = regexec(&regex, ping_answ[str_num], 0, NULL, 0);

        free(ping_answ[str_num]);

        if (!reg_ret)
            return 1;

    };

    return 0;
}

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

    /* Daemonize process */
    pid_t process_id = 0;
    pid_t sid = 0;

    // Create child process
    process_id = fork();

    if (process_id < 0)
    {
        err("Fork failed!");
        exit(1);
    }

    // PARENT PROCESS. Must be killed
    if (process_id > 0)
    {
        char started_msg[32];
        sprintf(started_msg, "Daemon started with pid: %d", process_id);
        wlog(started_msg, 1);

        printf("Daemon started (pid: %d)\n", process_id);
        exit(0);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Child process - "Daemon" code started from here */

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

    wlog("Daemon finished work", 1);
}


