#ifndef PROCESS_H //pour que ca s'inclut qu'une seule fois et eviter les erreures de redefinition
#define PROCESS_H

#include <unistd.h>
void afficher_processus();

void info_processus(pid_t pid);

void pause_processus(pid_t pid);
void reprise_processus(pid_t pid);
void arret_processus(pid_t pid);
void redemarrer_processus(pid_t pid);

#endif
