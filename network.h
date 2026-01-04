#ifndef NETWORK_H
#define NETWORK_H

#include <libssh/libssh.h>
#include <libtelnet.h>

#include <stddef.h>
#include <sys/types.h>

#define NETWORK_BUFFER_SIZE 8192

/* =========================================================
 * SSH
 * ========================================================= */

// Connexion SSH
ssh_session network_ssh_connect(
    const char *host,
    int port,
    const char *user,
    const char *password
);

// Exécuter une commande distante via SSH
int network_ssh_exec(
    ssh_session session,
    const char *command,
    char *output,
    size_t output_size
);

// Fermer la connexion SSH
void network_ssh_disconnect(ssh_session session);


/* =========================================================
 * TELNET
 * ========================================================= */
/*
typedef struct {
    int sockfd;
    telnet_t *telnet;
    char buffer[NETWORK_BUFFER_SIZE];
    size_t buffer_len;
} telnet_client_t;
*/

typedef struct {
    int sockfd;
    telnet_t *telnet;
} telnet_client_t;

void telnet_event_handler(telnet_t *telnet, telnet_event_t *ev, void *user_data);
void *reader_thread(void *arg);

telnet_client_t *telnet_connect(const char *ip, int port);



/*
// Connexion Telnet
telnet_client_t *network_telnet_connect(const char *host, int port);

// Envoi d'une commande Telnet
void network_telnet_send(telnet_client_t *client, const char *command);

// Traitement des données reçues Telnet
void network_telnet_event_handler(
    telnet_t *telnet,
    telnet_event_t *event,
    void *user_data
);

// Fermeture Telnet
void network_telnet_disconnect(telnet_client_t *client);
*/
#endif
