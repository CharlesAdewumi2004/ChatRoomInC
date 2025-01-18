#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 100

ClientInfo clients[MAX_CLIENTS]; // Global array to store client info
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Initializes the client list.
 */
void initializeClients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].sockfd = -1;
        memset(clients[i].username, 0, sizeof(clients[i].username));
    }
}

/**
 * @brief Validates and sets the client's username.
 */
int validateAndSetUsername(int sockfd, const char *username) {
    pthread_mutex_lock(&client_mutex);

    // Check for duplicate username
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd != -1 && strcmp(clients[i].username, username) == 0) {
            pthread_mutex_unlock(&client_mutex);
            printf("Username '%s' already taken.\n", username);
            return 0;
        }
    }

    // Add username to client list
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd == -1) {
            clients[i].sockfd = sockfd;
            strncpy(clients[i].username, username, sizeof(clients[i].username) - 1);
            clients[i].username[sizeof(clients[i].username) - 1] = '\0';
            pthread_mutex_unlock(&client_mutex);
            return 1;
        }
    }

    pthread_mutex_unlock(&client_mutex);
    printf("No available slots for client '%s'.\n", username);
    return 0;
}

/**
 * @brief Removes a client from the client list.
 */
void removeClient(int sockfd) {
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd == sockfd) {
            clients[i].sockfd = -1;
            memset(clients[i].username, 0, sizeof(clients[i].username));
            break;
        }
    }
    pthread_mutex_unlock(&client_mutex);
    printf("Client on sockfd %d removed.\n", sockfd);
}

/**
 * @brief Handles setting a username for the client.
 */
int attemptToSetUsername(int sockfd) {
    char username[100];
    char response[10];

    while (1) {
        int bytes_received = recv(sockfd, username, sizeof(username) - 1, 0);
        if (bytes_received > 0) {
            username[bytes_received] = '\0'; // Null-terminate
            if (validateAndSetUsername(sockfd, username)) {
                printf("Client '%s' connected on sockfd %d.\n", username, sockfd);
                strcpy(response, "1");
                send(sockfd, response, strlen(response), 0);
                return 1;
            } else {
                strcpy(response, "0");
                send(sockfd, response, strlen(response), 0);
            }
        } else {
            if (bytes_received == 0) {
                printf("Client on sockfd %d disconnected during username setup.\n", sockfd);
            } else {
                perror("Receive failed during username setup");
            }
            close(sockfd);
            return 0;
        }
    }
}

/**
 * @brief Receives messages from the client.
 */
void *recvMessages(void *arg) {
    int sockfd = *(int *)arg;
    free(arg);
    char buffer[300];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            printf("Message from client %d: %s\n", sockfd, buffer);
        } else if (bytesReceived == 0) {
            printf("Client on sockfd %d disconnected.\n", sockfd);
            close(sockfd);
            removeClient(sockfd);
            pthread_exit(NULL);
        } else {
            perror("Receive failed");
            close(sockfd);
            removeClient(sockfd);
            pthread_exit(NULL);
        }
    }
}

/**
 * @brief Handles a client connection.
 */
void *handleClientServer(void *arg) {
    int sockfd = *(int *)arg;

    if (attemptToSetUsername(sockfd)) {
        printf("Username set for client on sockfd %d.\n", sockfd);

        // Start receiving messages from the client
        recvMessages(arg);
    } else {
        printf("Failed to set username for client on sockfd %d. Disconnecting.\n", sockfd);
        close(sockfd);
    }

    pthread_exit(NULL);
}

/**
 * @brief Main server loop to handle incoming connections.
 */
void serverLoop(int serverSockfd) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    printf("Server is running. Waiting for connections...\n");

    while (1) {
        int clientSockfd = accept(serverSockfd, (struct sockaddr *)&client_addr, &addrlen);
        if (clientSockfd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("New client connected: %d\n", clientSockfd);

        pthread_t client_thread;
        int *sock_ptr = malloc(sizeof(int));
        *sock_ptr = clientSockfd;

        if (pthread_create(&client_thread, NULL, handleClientServer, sock_ptr) != 0) {
            perror("Thread creation failed");
            close(clientSockfd);
            free(sock_ptr);
        }
    }
}
