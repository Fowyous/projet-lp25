#include "process.h"



proc_info_t get_process_info_telnet(pid_t pid, serveurs *server){
	telnet_client_t *client = telnet_connect(server->addr, server->port);
	
	if (client == NULL){
		printf("erreur de connexion a l'addresse %s, port %d", server->addr, server->port);
		return NULL;
	}
	else {
		

	}
}
