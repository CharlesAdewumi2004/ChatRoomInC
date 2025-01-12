#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

ClientInfo clients[MAX_CLIENTS]; // Global array to store client info

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
 *
 * @param sockfd The client's socket file descriptor.
 * @param username The username to validate and set.
 * @return int Returns 1 on success, 0 if the username is invalid or already taken.
 */
int validateAndSetUsername(int sockfd, const char *username) {
    // Check if the username already exists
    printf("test %s\n", username);
   
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd != -1 && strcmp(clients[i].username, username) == 0) {
            printf("%d", strcmp(clients[i].username, username));
            printf("%d", sockfd);
            return 0; // Username already exists
        }
    }

    // Add the username to the clients list
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd == -1) { // Find an empty slot
            clients[i].sockfd = sockfd;
            strncpy(clients[i].username, username, sizeof(clients[i].username) - 1);
            clients[i].username[sizeof(clients[i].username) - 1] = '\0'; // Ensure null-termination
            return 1; // Success
        }
    }

    return 0; // No space available
}

/**
 * @brief Finds a client's username by their socket file descriptor.
 *
 * @param sockfd The socket file descriptor of the client.
 * @return const char* The username of the client, or NULL if not found.
 */
const char* findUsernameBySockfd(int sockfd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd == sockfd) {
            return clients[i].username;
        }
    }
    return NULL; // Not found
}

/**
 * @brief Removes a client from the client list.
 *
 * @param sockfd The socket file descriptor of the client to remove.
 */
void removeClient(int sockfd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd == sockfd) {
            clients[i].sockfd = -1; // Mark slot as unused
            memset(clients[i].username, 0, sizeof(clients[i].username));
            return;
        }
    }
}

/**
 * @brief Manages multiple attempts to set the username for the client.
 *
 * @param sockfd The client's socket file descriptor.
 * @return int Returns 1 if a username is successfully set, 0 if an error occurs.
 */
int attemptToSetUsername(int sockfd) {
    char message[100];
    char response[10];

    while (1) {
        // Receive the username attempt from the client
        int bytes_received = recv(sockfd, message, sizeof(message) - 1, 0);
        if (bytes_received > 0) {
            message[bytes_received] = '\0'; // Null-terminate the received message

            // Validate and set the username
            if (validateAndSetUsername(sockfd, message)) {
                printf("Added client '%s' with sockfd %d\n", message, sockfd);
                strcpy(response, "1"); // Success
                send(sockfd, response, strlen(response), 0);
                return 1; // Success
            } else {
                printf("Username '%s' is invalid or already taken\n", message);
                strcpy(response, "0"); // Failure
                send(sockfd, response, strlen(response), 0);
            }
        } else {
            perror("Receive failed or client disconnected");
            close(sockfd);
            return 0; // Error
        }
    }
}

/**
 * @brief Handles a client's connection and activities.
 *
 * @param sockfd The client's socket file descriptor.
 */
void handleClient(int sockfd) {

    printf("Handling client on socket: %d\n", sockfd);

    if (attemptToSetUsername(sockfd)) {
        const char *username = findUsernameBySockfd(sockfd);
        printf("Client with sockfd %d successfully set username '%s'\n", sockfd, username);
    } else {
        printf("Failed to set username for client on sockfd %d\n", sockfd);
        close(sockfd); // Close the socket if username setup fails
        return;
    }

    // TODO: Add further handling for the client after setting username
}


