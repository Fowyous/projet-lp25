#include "network.h"

// --------------------------------------------------
// Création d'un serveur TCP
// --------------------------------------------------
int create_server(int port) {
    int server_fd;
    struct sockaddr_in addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    return server_fd;
}

// --------------------------------------------------
// Accepter un client
// --------------------------------------------------
int accept_client(int server_fd) {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
    if (client_fd < 0) {
        perror("accept");
        return -1;
    }

    printf("Client connecté : %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    return client_fd;
}

// --------------------------------------------------
// Connexion à un serveur
// --------------------------------------------------
int connect_to_server(const char *ip, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

// --------------------------------------------------
// Envoi d'un message
// --------------------------------------------------
ssize_t send_message(int sockfd, const char *message) {
    return send(sockfd, message, strlen(message), 0);
}

// --------------------------------------------------
// Réception d'un message
// --------------------------------------------------
ssize_t receive_message(int sockfd, char *buffer, size_t size) {
    ssize_t bytes = recv(sockfd, buffer, size - 1, 0);
    if (bytes > 0)
        buffer[bytes] = '\0';
    return bytes;
}

// --------------------------------------------------
// Fermeture socket
// --------------------------------------------------
void close_socket(int sockfd) {
    close(sockfd);
}
