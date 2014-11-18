/*
    The library provides functions for reading and writing
	configuration files with info about hosts
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "../include/cmd.h"
#include "../include/raidix.h"
#include "../include/hosts.h"

/* Read and parse configuration file. Get info about host IP adresses and names */
void read_cfg()
{
    FILE *fp;

    char row[256];      // Current row of the config file
    int host_num = 0;   // Current number of the host

    fp = fopen("hosts.cfg", "r");

    if (!fp)
            err("Can't find configuration file: hosts.cfg");

    // Parse config file
    while (fgets(row, 256, fp)!=NULL)
    {
        // Info about every host must be separated by one empty line
        if (row[0] == '\n')
            host_num++;

        // Drop last '\n' char
        row[strlen(row)-1] = '\0';

        // Drop commented row
        if (row[0] == '#') {
            //commented line
        }

        // Extract NAME field
        else if (!memcmp(row, "NAME", 4))
        {
            hosts[host_num].cfg_name = (char *)malloc(strlen(&row[5])+1);
            strcpy(hosts[host_num].cfg_name, &row[5]);
        }

        // Extract IP field
        else if (!memcmp(row, "IP", 2))
        {
            hosts[host_num].ip = (char *)malloc(strlen(&row[3])+1);
            strcpy(hosts[host_num].ip, &row[3]);
        };
    };

    fclose(fp);
}

/* Put info about host to the file */
void save_host_info(int host_id)
{
    FILE *fp;

    char filename[16];
    sprintf(filename, "host_%d.info", host_id);

    fp = fopen(filename, "w");

    refresh_info_file(fp, host_id);

    fclose(fp);
}

/* Delete all hosts files */
void clean_hosts_info()
{
    char *exec_answ[32];
    int exec_answ_str_cnt = 0;

	exec_cmd("rm *.info", exec_answ, &exec_answ_str_cnt);
}

