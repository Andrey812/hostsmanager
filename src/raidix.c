/*
    The library provides functions for manage of the Raidix(c) hosts
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/raidix.h"

/* Parse Raidix version string and return major and minor
parts of the version
Example: raidix-core-4.2.0-657
Type: core (km, ui)
Major string: 4.2.0
Minor string: 657
*/
void extr_versions(char *str, char *type, char *maj, char *min)
{
int i_str       = 0;    // Counter for index of the char of the version string
int cnt_dash    = 0;    // Counter of the dashes in the version string ('-')
int cnt_point   = 0;    // Counter of the points in the version string ('.')

int i_type      = 0;    // Index of the char of type string
int i_min       = 0;    // Index of the char of min string
int i_maj       = 0;    // Index of the char of maj string

while(str[i_str] != '\0')
{
    if (str[i_str] == '-')
        cnt_dash++;

    else if (str[i_str] == '.')
        cnt_point++;

    // Type
    if (cnt_dash == 1 && cnt_point == 0 && str[i_str] != '-')
        type[i_type++] = str[i_str];

    // Major
    else if(cnt_dash == 2 && cnt_point < 3 && str[i_str] != '-')
    {
        maj[i_maj++] = str[i_str];
    }
    else if(cnt_dash == 2 && cnt_point > 2 && str[i_str] != '.')
    {
        min[i_min++] = str[i_str];
    }

    i_str++;
}

type[i_type++] = '\0';
maj[i_maj++] = '\0';
min[i_min++] = '\0';
}

/* Get raidix versions
Return:
0 - Can't take a version
1 - Version detected
*/
void ver(char *host, int host_num)
{
    int str_num;

    // Build get version command string
    char *cmd_tpl = "sshpass -p raidix ssh -t root@{0} 'rpm -qa | grep raidix' 2>/dev/null";
    char *cmd_attrs[] = {host};
    char cmd[256];

    make_cmd(cmd_tpl, cmd_attrs, cmd);

    // Execute version command and get answer
    char *ver_answ[32];
    int ver_answ_str_cnt = 0;

    exec_cmd(cmd, ver_answ, &ver_answ_str_cnt);

    for (str_num = 0; str_num < ver_answ_str_cnt; str_num++)
    {
        // Type (km, core, ui), maj and min parts of version
        char type[32] = "", maj[32] = "", min[32] = "";

        extr_versions(ver_answ[str_num], type, maj, min);

        if (!strcmp(type, "km"))
        {
            strcpy(hosts[host_num].km_ver_maj, maj);
            strcpy(hosts[host_num].km_ver_min, min);
        }

        else if (!strcmp(type, "core"))
        {
            strcpy(hosts[host_num].core_ver_maj, maj);
            strcpy(hosts[host_num].core_ver_min, min);
        }

        else if (!strcmp(type, "ui"))
        {
            strcpy(hosts[host_num].ui_ver_maj, maj);
            strcpy(hosts[host_num].ui_ver_min, min);
        }

        free(ver_answ[str_num]);
    };
}

