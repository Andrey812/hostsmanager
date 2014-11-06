/*
    Functions for processing external commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/flow.h"
#include "../include/cmd.h"

/*
    Execute external command and return array of strings
    with answer
*/
int exec_cmd(char *cmd_string, char *cmd_answ[], int *answ_str_cnt) {
    FILE *fp;
    int status;
    char str[256];

    fp = popen(cmd_string, "r");

    if (fp == NULL)
        err("Can't execute external program");

    int str_num = 0;
    while (fgets(str, 256, fp) != NULL)
    {
        int len;
        len = strlen(str);

        cmd_answ[str_num] = (char *)malloc(len + 1);
        strcpy(cmd_answ[str_num], str);
        str_num++;
    }

    // Set count of received strings
    *answ_str_cnt = str_num;

    status = pclose(fp);

    if (status == -1)
    {
        err("Can't close external program");
        return 0;
    }

    return 1;
}

/* Construct command string from template using array of attributes */
void make_cmd(char *cmd_tpl, char *attrs[], char *cmd)
{
    int i_tpl = 0;
    int i_cmd = 0;

    while(cmd_tpl[i_tpl] != '\0')
    {
        if (cmd_tpl[i_tpl] == '{' && cmd_tpl[i_tpl+2] == '}')
        {
            char *curr_attr = attrs[atoi(&cmd_tpl[i_tpl+1])];

            int ca_i = 0;

            while (curr_attr[ca_i] != '\0') {
                cmd[i_cmd] = curr_attr[ca_i];
                ca_i++;
                i_cmd++;
            }
            i_tpl = i_tpl + 3;
        }

        cmd[i_cmd] = cmd_tpl[i_tpl];
        i_tpl++;
        i_cmd++;
    }

    cmd[i_cmd] = '\0';
}

