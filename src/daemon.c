#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../include/cmd.h"
#include "../include/flow.h"
#include "../include/hosts.h"

#include "../include/raidix.h"

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


