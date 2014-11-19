/*
    Manage of the Raidix(c) hosts configuration
*/

#define RAIDIX_H_INCLUDED

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
    char hostname[256];     // Received hostname
};
struct host hosts[16];

void extr_versions(char *str, char *type, char *maj, char *min);
void ver(char *host, int host_num);
void hostname(char *host, int host_num);
void refresh_info_file(FILE *fp, int host_id);
