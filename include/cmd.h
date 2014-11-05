#define CMD_H_INCLUDED
int exec_cmd(char *cmd_string, char *cmd_answ[], int *answ_str_cnt);
void make_cmd(char *cmd_tpl, char *attrs[], char *cmd);
