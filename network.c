#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* =========================================================
 * SSH
 * ========================================================= */

ssh_session network_ssh_connect(
    const char *host,
    int port,
    const char *user,
    const char *password
) {
    ssh_session session = ssh_new();
    if (!session)
        return NULL;

    ssh_options_set(session, SSH_OPTIONS_HOST, host);
    ssh_options_set(session, SSH_OPTIONS_PORT, &port);
    ssh_options_set(session, SSH_OPTIONS_USER, user);

    if (ssh_connect(session) != SSH_OK) {
        fprintf(stderr, "SSH connect error: %s\n",
                ssh_get_error(session));
        ssh_free(session);
        return NULL;
    }

    if (ssh_userauth_password(session, NULL, password)
        != SSH_AUTH_SUCCESS) {
        fprintf(stderr, "SSH auth error: %s\n",
                ssh_get_error(session));
        ssh_disconnect(session);
        ssh_free(session);
        return NULL;
    }

    return session;
}

int network_ssh_exec(
    ssh_session session,
    const char *command,
    char *output,
    size_t output_size
) {
    if (!session || !command || !output)
        return -1;

    ssh_channel channel = ssh_channel_new(session);
    if (!channel)
        return -1;

    if (ssh_channel_open_session(channel) != SSH_OK)
        goto error;

    if (ssh_channel_request_exec(channel, command) != SSH_OK)
        goto error;

    int total = 0;
    int nbytes;

    while ((nbytes = ssh_channel_read(
                channel,
                output + total,
                output_size - total - 1,
                0)) > 0) {
        total += nbytes;
    }

    output[total] = '\0';

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    return total;

error:
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    return -1;
}

void network_ssh_disconnect(ssh_session session) {
    if (!session)
        return;

    ssh_disconnect(session);
    ssh_free(session);
}

/* =========================================================
 * TELNET
 * ========================================================= */
/*
static const telnet_telopt_t telnet_options[] = {
    { TELNET_TELOPT_ECHO, TELNET_WILL, TELNET_DO },
    { TELNET_TELOPT_SGA,  TELNET_WILL, TELNET_DO },
    { -1, 0, 0 }
};
*/
void telnet_event_handler(telnet_t *telnet, telnet_event_t *ev, void *user_data) {
    switch (ev->type) {
        case TELNET_EV_DATA:
            fwrite(ev->data.buffer, 1, ev->data.size, stdout);
            fflush(stdout);
            break;

        case TELNET_EV_SEND:
            send(((telnet_client_t*)user_data)->sockfd, ev->data.buffer, ev->data.size, 0);
            break;

        case TELNET_EV_ERROR:
            fprintf(stderr, "Telnet error: %s\n", ev->error.msg);
            exit(1);
    }
}



void *reader_thread(void *arg) {
    telnet_client_t *client = (telnet_client_t*)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        int n = recv(client->sockfd, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            printf("\nDisconnected from server.\n");
            exit(0);
        }
        telnet_recv(client->telnet, buffer, n);
    }
}


telnet_client_t *telnet_connect(const char *ip, int port) {
    telnet_client_t *client = malloc(sizeof(telnet_client_t));

    client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sockfd < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (connect(client->sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }

    printf("Connected.\n");

    static const telnet_telopt_t telopts[] = {
        { TELNET_TELOPT_ECHO, TELNET_WILL, TELNET_DO },
        { TELNET_TELOPT_TTYPE, TELNET_WILL, TELNET_DO },
        { TELNET_TELOPT_NAWS, TELNET_WILL, TELNET_DO },
        { -1, 0, 0 }
    };

    client->telnet = telnet_init(telopts, telnet_event_handler, 0, client);

    return client;
}

char *telnet_exec(telnet_client_t *client, const char *cmd) {
    memset(telnet_last_output, 0, sizeof(telnet_last_output));
    telnet_output_ready = 0;

    telnet_send(client->telnet, cmd, strlen(cmd));
    telnet_send(client->telnet, "\n", 1);

    // Wait for server output (simple delay)
    usleep(200000); // 200 ms

    return telnet_last_output;
}


/*
telnet_client_t *network_telnet_connect(const char *host, int port) {
    telnet_client_t *client = calloc(1, sizeof(telnet_client_t));
    if (!client)
        return NULL;

    client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sockfd < 0)
        goto error;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);

    if (connect(client->sockfd,
                (struct sockaddr *)&addr,
                sizeof(addr)) < 0)
        goto error;

    client->telnet = telnet_init(
        telnet_options,
        network_telnet_event_handler,
        0,
        client
    );

    if (!client->telnet)
        goto error;

    return client;

error:
    if (client->sockfd >= 0)
        close(client->sockfd);
    free(client);
    return NULL;
}

void network_telnet_send(telnet_client_t *client, const char *command) {
    if (!client || !command)
        return;

    telnet_send(client->telnet, command, strlen(command));
    telnet_send(client->telnet, "\n", 1);

    char recvbuf[512];
    ssize_t len;

    while ((len = recv(client->sockfd, recvbuf,
                       sizeof(recvbuf), MSG_DONTWAIT)) > 0) {
        telnet_recv(client->telnet, recvbuf, len);
    }
}

void network_telnet_disconnect(telnet_client_t *client) {
    if (!client)
        return;

    telnet_free(client->telnet);
    close(client->sockfd);
    free(client);
}*/
