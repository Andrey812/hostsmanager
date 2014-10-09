#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main()
{
    FILE *fp;
    int status;
    char str[255];

    fp = popen("sshpass -p raidix ssh root@172.16.4.118", "r");

    if (fp == NULL)
        printf("Can't execute external program");

    while (fgets(str, 255, fp) != NULL)
    {
        printf("%s\n", str);
    }

    status = pclose(fp);

    if (status == -1)
    {
        printf("Can't close external program");
    }
}

