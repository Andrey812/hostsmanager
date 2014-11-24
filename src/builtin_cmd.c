/*
    Prepared commands for local host
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../include/flow.h"
#include "../include/cmd.h"
#include "../include/builtin_cmd.h"

/*
    Process ping and extract result
    Returns:
    0 - host is unavailable
    1 - host is available
*/
int ping(char *host) {
    int reg_ret, str_num, reg_err;

    // Build ping command string with attributes
    char *cmd_tpl = "ping {0} -c 2 -W 4";
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
        wlog("Could not compile regex", 1);

    for (str_num = 0; str_num < ping_answ_str_cnt; str_num++)
    {
        reg_ret = regexec(&regex, ping_answ[str_num], 0, NULL, 0);

        free(ping_answ[str_num]);

        if (!reg_ret)
            return 1;

    };

    return 0;
}
