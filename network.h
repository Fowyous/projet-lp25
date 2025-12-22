#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// Création d'un serveur TCP
int create_server(int port);

// Accepter un client
int accept_client(int server_fd);

// Connexion à un serveur TCP
int connect_to_server(const char *ip, int port);

// Envoi et réception
ssize_t send_message(int sockfd, const char *message);
ssize_t receive_message(int sockfd, char *buffer, size_t size);

// Fermeture socket
void close_socket(int sockfd);

#endif
