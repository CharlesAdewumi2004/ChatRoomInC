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

#define PORT 8003

/**
 * @brief Thread function to handle client connections.
 *
 * @param arg Pointer to the client's socket file descriptor.
 * @return NULL
 */
void *handleClientThread(void *arg) {
    int clientSockfd = *(int *)arg; // Dereference pointer to get client socket
    free(arg); // Free dynamically allocated memory

    printf("Started thread for client socket: %d\n", clientSockfd);

    // Pass the socket to the client handler function
    handleClientServer((void *)&clientSockfd);

    // Close client socket after handling
    printf("Closing client socket: %d\n", clientSockfd);
    close(clientSockfd);

    // Exit the thread
    pthread_exit(NULL);
}



/**
 * @brief Starts the server.
 */
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
            close(clientSockfd); // Close client socket to avoid leaks
            continue;
        }
        *clientSockfdPtr = clientSockfd;

        if (pthread_create(&thread_id, NULL, handleClientThread, clientSockfdPtr) != 0) {
            perror("Failed to create thread");
            free(clientSockfdPtr); // Free allocated memory on failure
            close(clientSockfd);  // Close client socket
            continue;
        }

        // Detach thread to automatically clean up resources when it finishes
        pthread_detach(thread_id);
    }

    close(serverSockfd); // Close the server socket
}

/**
 * @brief Starts the client.
 */
void client() {
    struct sockaddr_in *config = getClientConfig(PORT, "127.0.0.1");
    int clientSockfd = createTCPSocket();
    if (connectClientSocket(clientSockfd, config) == -1) {
        perror("Failed to connect to server");
        return;
    }

    handleServer(clientSockfd);
    close(clientSockfd);
}

/**
 * @brief Main entry point of the program.
 *
 * @return int Returns 0 on success.
 */
int main() {
    int choice;

    // Prompt the user to select between running the server or client
    printf("Enter 1 to run the server or 2 to run the client: ");
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a number.\n");
        return EXIT_FAILURE;
    }

    while (getchar() != '\n'); // Clear the input buffer

    if (choice == 1) {
        printf("Starting server...\n");
        server();
    } else if (choice == 2) {
        printf("Starting client...\n");
        client();
    } else {
        printf("Invalid choice. Please enter 1 for server or 2 for client.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
