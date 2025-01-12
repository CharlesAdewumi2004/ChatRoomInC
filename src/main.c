#include "client.h"
#include "network.h"
#include "server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8005

void *clientThread(void *arg) {
    int clientSockfd = *(int *)arg; // Dereference the pointer to get the client socket
    free(arg); // Free the dynamically allocated memory

    handleClient(clientSockfd); // Process the client request
    close(clientSockfd); // Close the client socket after handling
    return NULL;
}

void server() {
    initializeClients();
    struct sockaddr_in *config = getServerConfig(PORT);
    int serverSockfd = createTCPSocket();
    bindSocket(serverSockfd, config);
    setSocketToListen(serverSockfd, 5);

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        // Accept a new client connection
        int clientSockfd = handleClientConnectionAttempt(serverSockfd);
        if (clientSockfd < 0) {
            perror("Failed to accept client connection");
            continue;
        }

        // Create a new thread for the client
        pthread_t thread_id;
        int *clientSockfdPtr = malloc(sizeof(int)); // Allocate memory for client socket
        if (clientSockfdPtr == NULL) {
            perror("Malloc failed");
            close(clientSockfd); // Close the client socket to avoid leaks
            continue;
        }
        *clientSockfdPtr = clientSockfd;

        if (pthread_create(&thread_id, NULL, clientThread, clientSockfdPtr) != 0) {
            perror("Failed to create thread");
            free(clientSockfdPtr); // Free allocated memory on failure
            close(clientSockfd);  // Close the client socket
            continue;
        }

        // Detach the thread so resources are released automatically when it finishes
        pthread_detach(thread_id);
    }

    close(serverSockfd); // Close the server socket
}


void client() {
  struct sockaddr_in *config = getClientConfig(PORT, "127.0.0.1");
  int clientSockfd = createTCPSocket();
  if (connectClientSocket(clientSockfd, config) == -1) {
    return;
  }
  handleServer(clientSockfd);
  close(clientSockfd);
}

int main() {
  int choice;
  printf("Enter 1 to run a server and 2 to run a client: \t");
  scanf("%d", &choice);
  while (getchar() != '\n')
    ;
  if (choice == 1) {
    server();
  } else {
    client();
  }
  return 0;
}
