#ifndef PROCESS_REMOTE_H
#define PROCESS_REMOTE_H

#include <sys/types.h>
#include <libssh/libssh.h>
#include "network.h"
#include <signal.h>

/* ================= SSH ================= */

// Pause processus distant
int pause_processus_distant_ssh(ssh_session session, pid_t pid);

// Reprise processus distant
int reprise_processus_distant_ssh(ssh_session session, pid_t pid);

// Arrêt processus distant
int arret_processus_distant_ssh(ssh_session session, pid_t pid);

// Redémarrage processus distant
int redemarrer_processus_distant_ssh(ssh_session session, pid_t pid);


/* ================= TELNET ================= */

proc_info_t get_process_info_telnet(telnet_client_t *client, pid_t pid);

int pause_processus_distant_telnet(telnet_client_t *client, pid_t pid);
int reprise_processus_distant_telnet(telnet_client_t *client, pid_t pid);
int arret_processus_distant_telnet(telnet_client_t *client, pid_t pid);

#endif
