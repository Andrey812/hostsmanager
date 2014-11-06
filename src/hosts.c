/*
    The library provides functions for reading and writing
	configuration files with info about hosts
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
            err("Can't find configuration file");

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

