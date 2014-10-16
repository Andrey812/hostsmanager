#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

/* Global structure with info about each host */
struct host
{
    // Params from config
    char *cfg_name;     // Name from configuration file
    char *ip;           // IP from cinfiguration file

    // Results of scan
    int ping;               // Ping result 0 or 1
    char km_ver_maj[64];    // KM version major ("4.2.0")
    char km_ver_min[64];    // KM version minor ("178")
    char core_ver_maj[64];  // CORE version major ("4.2.0")
    char core_ver_min[64];  // CORE version minor ("657")
    char ui_ver_maj[64];    // UI version major ("4.2.0")
    char ui_ver_min[64];    // UI version minor ("251")
    char hostname[255];     // Received hostname
};
struct host hosts[16];

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
        err("Could not compile regex");

    for (str_num = 0; str_num < ping_answ_str_cnt; str_num++)
    {
        reg_ret = regexec(&regex, ping_answ[str_num], 0, NULL, 0);

        free(ping_answ[str_num]);

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

/* The test functon for show of info about hosts */
void show_info()
{
    int host_num = 0;
    while(hosts[host_num].cfg_name != NULL)
    {
        printf("Id: %d\n", host_num);
        printf("Host: %s\n", hosts[host_num].cfg_name);
        printf("IP: %s\n", hosts[host_num].ip);
        printf("Ping: %d\n", hosts[host_num].ping);

        printf("KM: %s [%s]\n", hosts[host_num].km_ver_maj, hosts[host_num].km_ver_min);
        printf("CORE: %s [%s]\n", hosts[host_num].core_ver_maj, hosts[host_num].core_ver_min);
        printf("UI: %s [%s]\n", hosts[host_num].ui_ver_maj, hosts[host_num].ui_ver_min);

        printf("\n");
        host_num++;
    }
}

void main()
{
    // Get info about hosts from configuration file
    read_cfg();

    int host_num = 0;
    while(hosts[host_num].cfg_name != NULL)
    {
        // Ping
        hosts[host_num].ping = ping(hosts[host_num].ip);

        ver(hosts[host_num].ip, host_num);

        host_num++;
    };

    show_info();
}




