#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        error("Error opening socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_aton("127.0.0.1", &server_addr.sin_addr) == 0) {
        error("Invalid address");
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Connection failed");
    }

    printf("Connected to server. Type 'ping' to send a message, 'history' to view chat history, 'exit' to quit.\n");

    while (1) {
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);

        write(sock_fd, buffer, strlen(buffer));

        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = read(sock_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read > 0) {
            printf("Server: %s", buffer);
        } else {
            printf("Server disconnected.\n");
            break;
        }
    }

    close(sock_fd);
    return 0;
}
