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
    char str[255];

    fp = popen(cmd_string, "r");

    if (fp == NULL)
        err("Can't execute external program");

    int str_num = 0;
    while (fgets(str, 255, fp) != NULL)
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

/* make cmd string from template and attributes */
void make_cmd(char *cmd_tpl, char *attrs[], char *cmd)
{
    //TODO: Make buf size depending from length of cmd_tpl and attrs
    char buf[255];

    int i = 0;
    int i_buf = 0;
    while(cmd_tpl[i] != '\0')
    {
        if (cmd_tpl[i] == '{' && cmd_tpl[i+2] == '}')
        {
            char *curr_attr = attrs[atoi(&cmd_tpl[i+1])];
            int ca_i = 0;
            while (curr_attr[ca_i] != '\0') {
                buf[i_buf] = curr_attr[ca_i];
                ca_i++;
                i_buf++;
            }
            i = i + 3;
        }
        buf[i_buf] = cmd_tpl[i];
        i++;
        i_buf++;
    }
    buf[i_buf] = '\0';

    strcpy(cmd, buf);
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
    char cmd[255];

    make_cmd(cmd_tpl, cmd_attrs, cmd);

    // Execute ping command and get answer
    char *ping_answ[32];
    int *ping_answ_str_cnt;
    ping_answ_str_cnt = malloc(sizeof(int));

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
    char cmd[255];

    make_cmd(cmd_tpl, cmd_attrs, cmd);

    // Execute version command and get answer
    char *ver_answ[32];
    int *ver_answ_str_cnt;
    ver_answ_str_cnt = malloc(sizeof(int));

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

void main()
{
    char hosts[8][20]; // The list of ip adresses

    int host_num;
    int ping_result;

    //strcpy(hosts[0],"172.16.20.146");
    //strcpy(hosts[1],"172.16.20.150");
    //strcpy(hosts[2],"172.16.4.118");

    strcpy(hosts[0], "172.16.4.118");

    ver(hosts[0]);

    while(strlen(hosts[host_num]))
    {
        //printf("Ping result of %s : %d\n", hosts[host_num], ping(hosts[host_num]));

        host_num++;
    };
}

