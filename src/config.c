/*
    The library provides functions for reading and writing configuration files
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#include "../include/flow.h"
#include "../include/cmd.h"
#include "../include/config.h"

/* Read and parse main configuration file */
void read_cfg()
{
    FILE *fp;

    char row[256];      // Current row of the config file
    int section = 0;    // Current sections of the configuration file
                        // 0 - HOSTS, 1 - RULES, 2 - HOST_INFO 3 - SETTINGS
    int section_i = 0;  // Record number of the current section

    /* Try to use filename from arguments first
     or use default name of configuration file instead */
    if (!strlen(params.cfg_path))
    {
        sprintf(params.cfg_path, "%s", "hostman.cfg");
        sprintf(app.log, "Default configuration file '%s' was used", params.cfg_path);
        wlog(2,0);
    }

    fp = fopen(params.cfg_path, "r");

    if (!fp) {
        sprintf(app.log, "Can't open configuration file: %s", params.cfg_path);
        err();
    }

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
        else if(!memcmp(row, "[SETTINGS]", 10))
        {
            section = 3;
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

                // BLK field
                else if (!memcmp(row, "LOC", 3))
                {
                    if (!memcmp(&row[4], "1", 1))
                        scan_rules[section_i].loc = 1;
                }

                break;
            // SETTINGS
            case 3:

                if (!memcmp(row, "PID_PATH", 8))
                {
                    sprintf(params.pid_path, "%s", &row[9]);
                }

                else if (!memcmp(row, "LOG_PATH", 8))
                {
                    sprintf(params.log_path, "%s", &row[9]);
                }

                else if (!memcmp(row, "RES_PATH", 8))
                {
                    sprintf(params.res_path, "%s", &row[9]);
                }

                else if (!memcmp(row, "SCAN_PERIOD", 11))
                {
                    params.scan_period = atoi(&row[12]);
                }

                else if (!memcmp(row, "DEBUG", 5))
                {
                    if (!memcmp(&row[6], "1", 1))
                        params.debug = 1;
                }
        };
    };

    // Warinings
    if (!strlen(params.pid_path))
    {
        sprintf(params.pid_path, "hostman.pid");
        sprintf(app.log, "Pid path was not defined. Current path will be used. File: hostman.pid.");
        wlog(2,0);
    };

    if (!strlen(params.log_path))
    {
        sprintf(params.log_path, "hostman.log");
        sprintf(app.log, "Log path was not defined. Current path will be used. File: hostman.log");
        wlog(2,0);
    };

    if (!strlen(params.res_path))
    {
        sprintf(app.log, "Res path was not defined. Current path will be used. Files: <host_id>.info");
        wlog(2,0);
    }
    else {
        // Add "/" at the end of path if there is no such char
        if (params.res_path[strlen(params.res_path)-1] != '/') {
            sprintf(params.res_path, "%s/", params.res_path);
        }
    }


    fclose(fp);
}

/* Put info about host to the file */
void save_host_info(int host_id)
{
    FILE *fp;

    char filename[256];

    if (strlen(params.res_path))
    {
        sprintf(filename, "%shost_%d.info", params.res_path, host_id);
    }
    else
    {
        sprintf(filename, "host_%d.info", host_id);
    };

    fp = fopen(filename, "w");

    refresh_info_file(fp, host_id);

    fclose(fp);
}

/* Delete all hosts files */
void clean_hosts_info()
{
    char cmd[284];
    char *exec_answ[32];

    if (strlen(params.res_path))
    {
        sprintf(cmd, "rm %s*.info 2>/dev/null", params.res_path);
    }
    else
    {
        sprintf(cmd, "rm %s*.info 2>/dev/null", params.res_path);
    };

    int exec_answ_str_cnt = 0;

	exec_cmd(cmd, exec_answ, &exec_answ_str_cnt);
}

/* Refresh host info using received values */
void refresh_info_file(FILE *fp, int host_id)
{
    int rn = 0; // Number of the rule
    while(scan_rules[rn].cmd != NULL)
    {
        // 1 - field name, 2 - value
        if (strlen(scan_rules[rn].res) != 0)
        {
            fprintf(fp, "%s=%s\n", scan_rules[rn].res_field, scan_rules[rn].res);
            sprintf(scan_rules[rn].res, "%s", "");
        }

        rn++;
    };

    fprintf(fp, "ip=%s\n", hosts[host_id].ip);
    fprintf(fp, "updatetime=%ld\n", time(NULL));
}

