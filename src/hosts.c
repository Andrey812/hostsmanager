/*
    The library provides functions for reading and writing
	configuration files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#include "../include/flow.h"
#include "../include/cmd.h"
#include "../include/hosts.h"

/* Read and parse configuration file. Get info about host IP adresses and names */
void read_cfg()
{
    FILE *fp;

    char row[256];      // Current row of the config file
    int section = 0;    // Current sections of the configuration file
                        // 0 - HOSTS, 1 - RULES, 2 - HOST_INFO
    int section_i = 0;  // Record number of the current section

    fp = fopen("rules.cfg", "r");

    if (!fp)
            err("Can't find configuration file: rules.cfg");

    // Parse config file
    while (fgets(row, 256, fp)!=NULL)
    {
        // Drop commented row
        if (row[0] == '#')
        {
            // Nothing
        }

        // Switch section
        else if (!memcmp(row, "[HOSTS]", 7))
        {
            section = 0;
            section_i = 0;
        }
        else if(!memcmp(row, "[RULES]", 7))
        {
            section = 1;
            section_i = 0;
        }
        else if(!memcmp(row, "[HOST_INFO]", 11))
        {
            section = 2;
            section_i = 0;
        }

        // Section blocks
        else
        {
            // Info about every host must be separated by one empty line
            if (row[0] == '\n')
                section_i++;

            // Drop last '\n' char
            row[strlen(row)-1] = '\0';
        }

        switch(section)
        {
            // HOSTS
            case 0:
                // IP field
                if (!memcmp(row, "IP", 2))
                {
                    hosts[section_i].ip = (char *)malloc(strlen(&row[3])+1);
                    sprintf(hosts[section_i].ip, "%s", &row[3]);
                }

                // LOGIN field
                else if (!memcmp(row, "LOGIN", 5))
                {
                    hosts[section_i].login = (char *)malloc(strlen(&row[6])+1);
                    sprintf(hosts[section_i].login, "%s", &row[6]);
                }

                // PASSWORD field
                else if (!memcmp(row, "PASSWORD", 8))
                {
                    hosts[section_i].password = (char *)malloc(strlen(&row[9])+1);
                    sprintf(hosts[section_i].password, "%s", &row[9]);
                }

                break;
            // RULES
            case 1:
                // CMD field
                if (!memcmp(row, "CMD", 3))
                {
                    scan_rules[section_i].cmd = (char *)malloc(strlen(&row[4])+1);
                    sprintf(scan_rules[section_i].cmd, "%s", &row[4]);
                }

                // RES field
                else if (!memcmp(row, "RES", 3))
                {
                    scan_rules[section_i].res_field = (char *)malloc(strlen(&row[4])+1);
                    sprintf(scan_rules[section_i].res_field, "%s", &row[4]);
                }

                // BLK field
                else if (!memcmp(row, "BLK", 3))
                {
                    if (!memcmp(&row[4], "1", 1))
                        scan_rules[section_i].blk = 1;
                }

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

/* Refresh host info using received values */
void refresh_info_file(FILE *fp, int host_id)
{
    int rn = 0; // Number of the rule
    while(scan_rules[rn].cmd != NULL)
    {
        // 1 - field name, 2 - value
        if (scan_rules[rn].res != NULL)
            fprintf(fp, "%s=%s\n", scan_rules[rn].res_field, scan_rules[rn].res);

        // Clear memory for the next iteration
        free(scan_rules[rn].res);

        rn++;
    };

    fprintf(fp, "IP=%s\n", hosts[host_id].ip);
    fprintf(fp, "UPDATETIME=%ld\n", time(NULL));
}

