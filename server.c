#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define HISTORY_FILE "chat_history.txt"

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}


void append_to_history(const char *message) {
    FILE *file = fopen(HISTORY_FILE, "a");
    if (!file) {
        perror("Error opening history file");
        return;
    }
    fprintf(file, "%s", message);
    fclose(file);
}


void send_history(int client_fd) {
    FILE *file = fopen(HISTORY_FILE, "r");
    if (!file) {
        const char *error_msg = "No chat history available.\n";
        write(client_fd, error_msg, strlen(error_msg));
        return;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        write(client_fd, line, strlen(line));
    }
    fclose(file);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        error("Error opening socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Error binding socket");
    }

    if (listen(server_fd, 5) < 0) {
        error("Error on listen");
    }

    printf("Server is listening on port %d...\n", PORT);

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
        error("Error on accept");
    }
    printf("Client connected.\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Received: %s", buffer);
        append_to_history(buffer);

        if (strncmp(buffer, "ping", 4) == 0) {
            const char *response = "pong\n";
            write(client_fd, response, strlen(response));
            printf("Sent: pong\n");
        } else if (strncmp(buffer, "history", 7) == 0) {
            send_history(client_fd);
        } else {
            const char *response = "Unknown command\n";
            write(client_fd, response, strlen(response));
        }
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
