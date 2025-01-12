#include "network.h"
#include "client.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_USERNAME_LENGTH 50
#define MAX_RETRIES 3

/**
 * @brief Handles sending and receiving usernames from the server.
 *
 * @param sockfd Socket file descriptor.
 */
void handleServer(int sockfd) {
    int retry_count = 0;

    while (retry_count < MAX_RETRIES) {
        if (setUsername(sockfd)) {
            printf("Username set successfully!\n");
            return; // Exit after successful setup
        } else {
            printf("Failed to set username. Attempts remaining: %d\n", MAX_RETRIES - retry_count - 1);
            retry_count++;
        }
    }

    printf("Maximum retry attempts reached. Disconnecting...\n");
    close(sockfd); // Disconnect the client if retries are exhausted
    exit(EXIT_FAILURE);
}

/**
 * @brief Sends the username to the server and validates the response.
 *
 * @param sockfd Socket file descriptor.
 * @return int Returns 1 if the username is valid, 0 otherwise.
 */
int setUsername(int sockfd) {
    char username[MAX_USERNAME_LENGTH + 1];
    char response[2]; // Response buffer to handle "1\0" or "0\0"

    // Prompt the user for a username
    printf("Enter username:\t");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0'; // Remove trailing newline

    // Validate username length
    if (strlen(username) == 0) {
        printf("Username cannot be empty. Please try again.\n");
        return 0;
    } else if (strlen(username) > MAX_USERNAME_LENGTH) {
        printf("Username too long (max %d characters). Please try again.\n", MAX_USERNAME_LENGTH);
        return 0;
    }

    // Send the username to the server
    if (send(sockfd, username, strlen(username), 0) < 0) {
        perror("Send failed");
        return 0;
    }

    // Receive the server's response
    int bytes_received = recv(sockfd, response, sizeof(response) - 1, 0);
    if (bytes_received > 0) {
        response[bytes_received] = '\0'; // Null-terminate the response

        // Check the server's response
        if (strcmp(response, "1") == 0) {
            return 1; // Username is valid
        } else {
            printf("Server rejected username. It might be taken or invalid.\n");
            return 0; // Username is invalid
        }
    } else if (bytes_received == 0) {
        printf("Server disconnected.\n");
        return 0;
    } else {
        perror("Receive failed");
        return 0;
    }
}

