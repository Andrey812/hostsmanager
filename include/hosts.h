/*
    Read and write configuration files with info about hosts
*/

#define HOSTS_H_INCLUDED

/* Global structure with info about each host */
void read_cfg();
void save_host_info(int);
void clean_hosts();
