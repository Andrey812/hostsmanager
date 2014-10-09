#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

/* Process errors (Prints error message to STDERR and exit */
void err(char *err_str) {
    fprintf(stderr, "Error happened: '%s'\n", err_str);
    exit(0);
}

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
    int *ping_answ_str_cnt;
    *ping_answ_str_cnt = 0;

    exec_cmd(cmd, ping_answ, ping_answ_str_cnt);

    // Parse ping answer
    regex_t regex; //regex for parsing of the ping answer
    reg_err = regcomp(&regex, "time=", 0);
    if (reg_err)
        err("Could not compile regex");

    for (str_num = 0; str_num < *ping_answ_str_cnt; str_num++)
    {
        reg_ret = regexec(&regex, ping_answ[str_num], 0, NULL, 0);
        if (!reg_ret)
            return 1;
    };

    return 0;
}

/* Get raidix versions
Return:
0 - Can't take a version
1 - Version detected
*/
int ver(char *host)
{
    int reg_ret, str_num, reg_err;

    // Build get version command string
    char *cmd_tpl = "sshpass -p raidix ssh -t root@{0} 'rpm -qa | grep raidix' 2>/dev/null";
    char *cmd_attrs[] = {host};
    char cmd[256];

    make_cmd(cmd_tpl, cmd_attrs, cmd);

    // Execute version command and get answer
    char *ver_answ[32];
    int *ver_answ_str_cnt;
    *ver_answ_str_cnt = 0;

    exec_cmd(cmd, ver_answ, ver_answ_str_cnt);

    // Parse version answer
    regex_t regex;
    reg_err = regcomp(&regex, "time=", 0);
    if (reg_err)
        err("Could not compile regex");

    for (str_num = 0; str_num < *ver_answ_str_cnt; str_num++)
    {
        printf("%s\n", ver_answ[str_num]);
        //reg_ret = regexec(&regex, ping_answ[str_num], 0, NULL, 0);
        //if (!reg_ret)
        //    return 0;
    };

    return 1;
}

/* Global structure with info about each host */
struct host {
    char *name[256];
    char *ip[16];
};

struct host hosts[5];

/* Read and parse configuration file. Get info about host IP adresses and names */
void read_cfg()
{
    FILE *fp;

    char row[256];              // Current row from config file
    char host_name[256];        // Buf for current host name
    char host_ip_address[16];   // Buf for current fost ip address

    fp = fopen("hosts.cfg", "r");

    if (!fp)
            err("Can't find configuration file");

    // Parse config file
    while (fgets(row, 256, fp)!=NULL)
    {
        if (!memcmp(row, "NAME", 4)) {
            strcpy(host_name, &row[5]);
            printf("%s", host_name);
        };
    };

    fclose(fp);
}


void main()
{

    /*
    *hosts[0].name = "host_1";
    *hosts[0].ip = "172.16.4.118";

    *hosts[1].name = "host_2";
    *hosts[1].ip = "172.16.4.118";

    *hosts[2].name = "host_3";
    *hosts[2].ip = "172.16.4.118";
    */

    read_cfg();

    //printf("%s\n", *hosts[2].ip);

/*
    while(strlen(hosts[host_num]))
    {
        printf("Ping result of %s : %d\n", hosts[host_num], ping(hosts[host_num]));

        host_num++;
    };
*/
}


