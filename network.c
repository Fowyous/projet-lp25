#include "network.h"
#include <sys/socket.h>
#include <netdb.h>

static const telnet_telopt_t telopts[] = {
    { TELNET_TELOPT_ECHO, TELNET_WILL, TELNET_DO },
    { TELNET_TELOPT_SGA,  TELNET_WILL, TELNET_DO },
    { -1, 0, 0 }
};

void telnet_event_handler(telnet_t *telnet, telnet_event_t *ev, void *user_data) {
    (void)telnet;
    if (ev->type == TELNET_EV_DATA) {
        write(STDOUT_FILENO, ev->data.buffer, ev->data.size);
    }
}

telnet_client_t *telnet_connect(const char *host, int port) {
    struct sockaddr_in addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("telnet connect");
        close(sockfd);
        return NULL;
    }

    telnet_client_t *client = malloc(sizeof(telnet_client_t));
    client->sockfd = sockfd;
    client->telnet = telnet_init(telopts, telnet_event_handler, 0, NULL);

    return client;
}

void telnet_send_command(telnet_client_t *client, const char *command) {
    telnet_send(client->telnet, command, strlen(command));
    telnet_send(client->telnet, "\n", 1);
}

void telnet_disconnect(telnet_client_t *client) {
    if (!client) return;
    telnet_free(client->telnet);
    close(client->sockfd);
    free(client);
}

