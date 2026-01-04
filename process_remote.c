#include "process_remote.h"
#include <stdio.h>
#include <string.h>

/* =========================================================
 * SSH
 * ========================================================= */

static int exec_kill_ssh(ssh_session session, int sig, pid_t pid) {
    char cmd[64];
    char buffer[256];

    snprintf(cmd, sizeof(cmd), "kill -%d %d", sig, pid);
    return network_ssh_exec(session, cmd, buffer, sizeof(buffer));
}

int pause_processus_distant_ssh(ssh_session session, pid_t pid) {
    return exec_kill_ssh(session, SIGSTOP, pid);
}

int reprise_processus_distant_ssh(ssh_session session, pid_t pid) {
    return exec_kill_ssh(session, SIGCONT, pid);
}

int arret_processus_distant_ssh(ssh_session session, pid_t pid) {
    return exec_kill_ssh(session, SIGTERM, pid);
}

int redemarrer_processus_distant_ssh(ssh_session session, pid_t pid) {
    char cmd[256];
    char buffer[256];

    /* Récupérer le chemin de l'exécutable distant */
    snprintf(cmd, sizeof(cmd),
             "readlink /proc/%d/exe", pid);

    if (network_ssh_exec(session, cmd, buffer, sizeof(buffer)) <= 0)
        return -1;

    buffer[strcspn(buffer, "\n")] = 0;

    /* Tuer et relancer */
    snprintf(cmd, sizeof(cmd),
             "kill %d && sleep 1 && %s &",
             pid, buffer);

    return network_ssh_exec(session, cmd, buffer, sizeof(buffer));
}

/* =========================================================
 * TELNET (pédagogique)
 * ========================================================= */

static int exec_kill_telnet(telnet_client_t *client, int sig, pid_t pid) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "kill -%d %d", sig, pid);
    network_telnet_send(client, cmd);
    return 0;
}

int pause_processus_distant_telnet(telnet_client_t *client, pid_t pid) {
    return exec_kill_telnet(client, SIGSTOP, pid);
}

int reprise_processus_distant_telnet(telnet_client_t *client, pid_t pid) {
    return exec_kill_telnet(client, SIGCONT, pid);
}

int arret_processus_distant_telnet(telnet_client_t *client, pid_t pid) {
    return exec_kill_telnet(client, SIGTERM, pid);
}
