#include "process.h"
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
 * TELNET
 * ========================================================= */

proc_info_t get_process_info_telnet(telnet_client_t *client, pid_t pid){
	proc_info_t info;
	memset(&info, 0, sizeof(info));
	info.pid = pid;

	char cmd[300];
	snprintf(cmd, sizeof(cmd), "ps -p %d -o pid=,user=,ppid=,stat=,etime=,pcpu=,pmem=,comm=", pid);

	char *output = telnet_exec(client, cmd);

	if (strlen(output) < 2){
		info.pid = -1;
		return info;
	}
 
	sscanf(output, "%d %63s %d %c %63s %lf %lf %255s",
			&info.pid, 
			info.user, 
			&info.ppid, 
			&info.state, 
			(char[64]){0},//e
			&info.cpu_percent, 
			&info.mem_percent, 
				info.name ); 
	return info;
}
static int exec_kill_telnet(telnet_client_t *client, int sig, pid_t pid) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "kill -%d %d\n", sig, pid);
    telnet_send(client->telnet, cmd, strlen(cmd));
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
