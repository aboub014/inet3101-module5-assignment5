#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 512

int main(int argc, char const *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <server_ip> <port> <filename>\n", argv[0]);
        return 1;
    }

    const char *serverIP = argv[1];
    int port = atoi(argv[2]);
    const char *filename = argv[3];

    int serverSocketID = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketID < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIP, &serverAddr.sin_addr) <= 0) {
        printf("Error: invalid IP address\n");
        close(serverSocketID);
        return 1;
    }

    int connectStatus = connect(serverSocketID, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (connectStatus == -1) {
        printf("Error: Could not connect to the server.\n");
        close(serverSocketID);
        return 1;
    }

    char sendBuf[256];
    snprintf(sendBuf, sizeof(sendBuf), "%s\n", filename);
    send(serverSocketID, sendBuf, strlen(sendBuf), 0);

    char serverMsg[BUF_SIZE];
    memset(serverMsg, 0, sizeof(serverMsg));
    int bytes = recv(serverSocketID, serverMsg, sizeof(serverMsg) - 1, 0);
    if (bytes > 0) {
        printf("%s", serverMsg);
        if (serverMsg[strlen(serverMsg) - 1] != '\n') printf("\n");
    } else {
        printf("No response from server.\n");
    }

    close(serverSocketID);
    return 0;
}