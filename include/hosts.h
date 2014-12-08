/*
    Settings from configuration file
    (daemon configuration, hosts scan rules)
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
    int  loc;           // Execute such shell command on the local host
};

// Up to 32 scanner commands
struct scan_rule scan_rules[32];

/* Application settings */
struct settings
{
    char pid_path[256];     // Path to the pid file
    char log_path[256];     // Path to the log file
    char res_path[256];     // Path to the directory of scan result files
    char cfg_path[256];     // Path to the main configuration file
    int scan_period;    // Rate of the scanning cycles (in seconds)
    int debug;          // If 1 - enable debug records to the log file
};

struct settings params;

/* Global structure with info about each host */
void read_cfg();
void save_host_info(int);
void clean_hosts_info();
void refresh_info_file();
