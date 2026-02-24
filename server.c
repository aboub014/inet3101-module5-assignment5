#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_CONTENT 255
#define MAX_FILENAME 256

static int serverSocketID = -1;

void handle_sigint(int sig) {
    (void)sig;
    printf("\nShutting down server...\n");
    if (serverSocketID != -1) close(serverSocketID);
    exit(0);
}

int valid_filename(const char *name) {
    if (name == NULL || name[0] == '\0') return 0;
    if (strstr(name, "..") != NULL) return 0;     // block traversal
    if (strchr(name, '/') != NULL) return 0;      // block folders
    return 1;
}

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    signal(SIGINT, handle_sigint);

    serverSocketID = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketID < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(serverSocketID, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocketID, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        close(serverSocketID);
        return 1;
    }

    if (listen(serverSocketID, 5) < 0) {
        perror("listen");
        close(serverSocketID);
        return 1;
    }

    printf("Server listening on port %d...\n", port);

    while (1) {
        int clientSocket = accept(serverSocketID, NULL, NULL);
        if (clientSocket < 0) {
            perror("accept");
            continue;
        }

        char filename[MAX_FILENAME];
        memset(filename, 0, sizeof(filename));
        int bytes = recv(clientSocket, filename, sizeof(filename) - 1, 0);
        if (bytes <= 0) {
            close(clientSocket);
            continue;
        }

        filename[strcspn(filename, "\r\n")] = '\0';

        if (!valid_filename(filename)) {
            char *msg = "ERROR: invalid filename\n";
            send(clientSocket, msg, strlen(msg), 0);
            close(clientSocket);
            continue;
        }

        FILE *fp = fopen(filename, "r");
        if (!fp) {
            char *msg = "ERROR: file not found\n";
            send(clientSocket, msg, strlen(msg), 0);
            close(clientSocket);
            continue;
        }

        char content[MAX_CONTENT + 1];
        int count = 0;
        int ch;

        while ((ch = fgetc(fp)) != EOF) {
            if (count >= MAX_CONTENT) {
                fclose(fp);
                char *msg = "ERROR: file too large (max 255 chars)\n";
                send(clientSocket, msg, strlen(msg), 0);
                close(clientSocket);
                goto done; 
            }
            content[count++] = (char)ch;
        }
        fclose(fp);
        content[count] = '\0';

        send(clientSocket, content, strlen(content), 0);

        close(clientSocket);

    done:
        ; 
    }

    return 0;
}