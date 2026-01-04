#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 23               // Default Telnet port
#define BUFFER_SIZE 1024
#define USERNAME "admin"      // Valid username
#define PASSWORD "password"   // Valid password

int authenticate(int client_fd) {
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];

    // Ask for username
    send(client_fd, "Username: ", 10, 0);
    memset(username, 0, sizeof(username));
    recv(client_fd, username, sizeof(username) - 1, 0);

    // Ask for password
    send(client_fd, "Password: ", 10, 0);
    memset(password, 0, sizeof(password));
    recv(client_fd, password, sizeof(password) - 1, 0);

    // Remove newline characters
    username[strcspn(username, "\n")] = 0;
    password[strcspn(password, "\n")] = 0;

    // Check credentials
    if (strcmp(username, USERNAME) == 0 && strcmp(password, PASSWORD) == 0) {
        send(client_fd, "Login successful\n", 17, 0);
        return 1; // Authentication successful
    } else {
        send(client_fd, "Login failed\n", 14, 0);
        return 0; // Authentication failed
    }
}

void execute_command(int client_fd, const char* command) {
    FILE *fp;
    char result[BUFFER_SIZE];

    // Open the command for reading
    fp = popen(command, "r");
    if (fp == NULL) {
        send(client_fd, "Failed to execute command\n", 26, 0);
        return;
    }

    // Read the output a line at a time - output it.
    while (fgets(result, sizeof(result), fp) != NULL) {
        send(client_fd, result, strlen(result), 0);
    }

    // Close the process
    pclose(fp);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Telnet server listening on port %d\n", PORT);

    // Accept a connection
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected\n");

        // Authenticate the user
        if (authenticate(client_fd)) {
            while (1) {
                memset(buffer, 0, BUFFER_SIZE);
                int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
                if (bytes_received <= 0) {
                    // Client disconnected
                    printf("Client disconnected\n");
                    break;
                }

                // Remove newline characters
                buffer[strcspn(buffer, "\n")] = 0;

                // Execute the command
                execute_command(client_fd, buffer);
            }
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}

