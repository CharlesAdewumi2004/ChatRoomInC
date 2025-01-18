#include "client.h"
#include "server.h"
#include "network.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void server() {
    initializeClients();
    struct sockaddr_in *config = getServerConfig(PORT);
    int serverSockfd = createTCPSocket();
    bindSocket(serverSockfd, config);
    setSocketToListen(serverSockfd, 5);

    while (1) {
        int clientSockfd = handleClientConnectionAttempt(serverSockfd);
        pthread_t thread_id;
        int *sock_ptr = malloc(sizeof(int));
        *sock_ptr = clientSockfd;
        pthread_create(&thread_id, NULL, handleClient, sock_ptr);
        pthread_detach(thread_id);
    }
}

void client() {
    struct sockaddr_in *config = getClientConfig(PORT, "127.0.0.1");
    int clientSockfd = createTCPSocket();
    if (connectClientSocket(clientSockfd, config) == -1) {
        perror("Failed to connect");
        return;
    }

    handleServer(clientSockfd);
    close(clientSockfd);
}

int main() {
    int choice;
    printf("Enter 1 to run the server or 2 to run the client: ");
    scanf("%d", &choice);

    if (choice == 1) {
        server();
    } else if (choice == 2) {
        client();
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}
