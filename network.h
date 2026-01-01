#ifndef NETWORK_H
#define NETWORK_H

#include <libssh/libssh.h>
#include <libssh/callbacks.h>
#include <libtelnet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

/* ===================== SSH ===================== */

// Connexion SSH
ssh_session ssh_connect(const char *host, int port, const char *user, const char *password);

// Exécution d'une commande distante
int ssh_execute_command(ssh_session session, const char *command, char *output, size_t out_size);

// Fermeture session SSH
void ssh_disconnect(ssh_session session);

/* ===================== TELNET ===================== */

// Structure Telnet
typedef struct {
    telnet_t *telnet;
    int sockfd;
} telnet_client_t;

// Connexion Telnet
telnet_client_t *telnet_connect(const char *host, int port);

// Envoi commande Telnet
void telnet_send_command(telnet_client_t *client, const char *command);

// Réception Telnet (callback)
void telnet_event_handler(telnet_t *telnet, telnet_event_t *ev, void *user_data);

// Fermeture Telnet
void telnet_disconnect(telnet_client_t *client);

#endif
