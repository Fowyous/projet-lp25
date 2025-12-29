#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>   // pid_t, gid_t

// Structure qui contient les infos d'un processus
typedef struct {
    pid_t  pid;
    char   user[64];
    gid_t  gid;
    char   name[256];
    char   state;            // 'R', 'S', 'D', 'Z', etc.
    pid_t  ppid;
    double uptime_seconds;
    double cpu_percent;
    double mem_percent;
} proc_info_t;

// Fonctions disponibles
void afficher_processus(void);
void rechercher_processus(const char *nom);

proc_info_t get_process_info(pid_t pid);
void info_processus(pid_t pid);

void pause_processus(pid_t pid);
void reprise_processus(pid_t pid);
void arret_processus(pid_t pid);
void redemarrer_processus(pid_t pid);

#endif
