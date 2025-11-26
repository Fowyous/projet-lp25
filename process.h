#ifndef PROCESS_H //pour que ca s'inclut qu'une seule fois et eviter les erreures de redefinition
#define PROCESS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>

#include <unistd.h>
typedef struct {
    int pid;
    int ppid;
    int gid;
    char user[64];
    char name[128];
    char state;
    double cpu_percent;
    double mem_percent;
    double uptime_seconds;
} proc_info_t;
void afficher_processus();

proc_info_t get_process_info(pid_t pid);

void pause_processus(pid_t pid);
void reprise_processus(pid_t pid);
void arret_processus(pid_t pid);
void redemarrer_processus(pid_t pid);

#endif
