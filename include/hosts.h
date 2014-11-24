/*
    Info about hosts for scanner
*/

#define HOSTS_H_INCLUDED

/* List of the hosts for scanning and their params */
struct host
{
    char *ip;           // Host IP
    char *login;        // SSH access username
    char *password;     // SSH access password
};

// Up to 16 hosts
struct host hosts[16];

/* List of the scanner commands which will be applied for each host */
struct scan_rule
{
    char *cmd;          // Builtin command or bash scanner command
    char *res_field;    // Result field name of the executed command
    char res[256];      // Result string (will be returned from executed command)
    int  blk;           // Blocking or unblocking command (depending from result)
};

// Up to 32 scanner commands
struct scan_rule scan_rules[32];

/* Global structure with info about each host */
void read_cfg();
void save_host_info(int);
void clean_hosts_info();
void refresh_info_file();
