/*
    Functions for processing the external commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/flow.h"
#include "../include/cmd.h"
#include "../include/config.h"
#include "../include/builtin_cmd.h"

/* Execute external command and return array of strings with answer */
int exec_cmd(char *cmd_string, char *cmd_answ[], int *answ_str_cnt) {
    FILE *fp;
    int status;
    char str[256];

    fp = popen(cmd_string, "r");

    if (fp == NULL)
    {
        sprintf(app.log, "Can't execute external program");
        wlog(0,0);
    }

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
        sprintf(app.log, "Can't close external program");
        wlog(0,0);
        return 0;
    }

    // Debug logs for command's flow

    // Save logs
    sprintf(app.log, "CMD SENT:[%s]", cmd_string);
    wlog(2,0);
    sprintf(app.log, "ANSWER RECEIVED:[%s]", str_num ? cmd_answ[0] : "");
    wlog(2,0);

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

/* Execute list of commands and save answers */
void execute_scan_rules(int host_num)
{
    int rn = 0; // Number of the rule in the rules list

    char cmd[256];
    char *hm_answ[256]; // Array of the strings
    int hm_answ_str_cnt;
    int mem_clr_i;

	while(scan_rules[rn].cmd != NULL)
    {
        hm_answ_str_cnt = 0;

        // Buildin command "ping"
        if (!memcmp(scan_rules[rn].cmd, "_ping", 5))
        {
            int ping_result = ping(hosts[host_num].ip);
            hm_answ_str_cnt = ping_result;
            sprintf(scan_rules[rn].res, "%d", ping_result);
        }

        else
        {
            // This command will be execute on the local node
            if (scan_rules[rn].loc == 1) {
                    sprintf(cmd, "%s", scan_rules[rn].cmd);
            }

            // This command will be executed on the remote node
            else {
                if (hosts[host_num].login != NULL && hosts[host_num].password != NULL)
                {
                    sprintf(cmd, "sshpass -p %s ssh -o ConnectTimeout=10 -t %s@%s '%s' 2>/dev/null",
                        hosts[host_num].password,
                        hosts[host_num].login,
                        hosts[host_num].ip,
                        scan_rules[rn].cmd
                    );
                }
                else
                {
                    sprintf(cmd, "ssh -o ConnectTimeout=10 -t %s '%s' 2>/dev/null",
                        hosts[host_num].ip,
                        scan_rules[rn].cmd
                    );
                }
            };

            exec_cmd(cmd, hm_answ, &hm_answ_str_cnt);

            if (hm_answ_str_cnt > 0)
            {
                // Drop last '\n' char
                hm_answ[0][strlen(hm_answ[0])-1] = '\0';

                // Write result
                sprintf(scan_rules[rn].res, "%s", hm_answ[0]);

                // Clear used memory
                for (mem_clr_i = 0; mem_clr_i < hm_answ_str_cnt; mem_clr_i++)
                {
                    free(hm_answ[mem_clr_i]);
                }
            }
        }

        // Drop execution of the rules list for current host
        // if current rule returned nothing and it has active blk flag
        if (hm_answ_str_cnt == 0 && scan_rules[rn].blk == 1)
            return;

        rn++;
    };
}
