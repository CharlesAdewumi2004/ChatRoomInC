#include "client.h"
#include "network.h"
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

pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Handles sending and receiving usernames from the server.
 *
 * @param sockfd Socket file descriptor.
 */
void handleServer(int sockfd) {
    int retry_count = 0;

    // Attempt to set the username
    while (retry_count < MAX_RETRIES) {
        if (setUsername(sockfd)) {
            printf("Username set successfully!\n");
            break;
        } else {
            printf("Failed to set username. Attempts remaining: %d\n", MAX_RETRIES - retry_count - 1);
            retry_count++;
        }
    }

    // Exit if the username setup fails after retries
    if (retry_count == MAX_RETRIES) {
        printf("Exceeded maximum attempts to set username. Exiting.\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Thread IDs for sending and receiving messages
    pthread_t send_thread, recv_thread;

    // Create a thread for receiving messages
    if (pthread_create(&recv_thread, NULL, recvMessage, &sockfd) != 0) {
        perror("Failed to create receive thread");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Create a thread for sending messages
    if (pthread_create(&send_thread, NULL, sendMessage, &sockfd) != 0) {
        perror("Failed to create send thread");
        close(sockfd);
        pthread_cancel(recv_thread); // Cancel the receive thread if send thread fails
        exit(EXIT_FAILURE);
    }

    // Wait for both threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    // Close the socket after threads are done
    close(sockfd);
}

/**
 * @brief Receives messages from the server in a loop.
 *
 * @param arg Pointer to the socket file descriptor.
 * @return void*
 */
void *recvMessage(void *arg) {
    int sockfd = *(int *)arg; // Extract socket file descriptor
    char responseBuffer[300];

    while (1) {
        memset(responseBuffer, 0, sizeof(responseBuffer));

        pthread_mutex_lock(&socket_mutex);
        int bytesReceived = recv(sockfd, responseBuffer, sizeof(responseBuffer) - 1, 0);
        pthread_mutex_unlock(&socket_mutex);

        if (bytesReceived > 0) {
            responseBuffer[bytesReceived] = '\0'; // Null-terminate the received message
            printf("Server: %s\n", responseBuffer);
        } else if (bytesReceived == 0) {
            printf("Server disconnected. Closing receive thread.\n");
            pthread_exit(NULL);
        } else {
            perror("Receive failed");
            pthread_exit(NULL);
        }
    }
}

/**
 * @brief Sends messages to the server in a loop.
 *
 * @param arg Pointer to the socket file descriptor.
 * @return void*
 */
void *sendMessage(void *arg) {
    int sockfd = *(int *)arg; // Extract socket file descriptor
    char messageBuffer[300];

    while (1) {
        fgets(messageBuffer, sizeof(messageBuffer), stdin);
        messageBuffer[strcspn(messageBuffer, "\n")] = '\0'; // Remove trailing newline

        if (strlen(messageBuffer) == 0) {
            printf("Invalid message. Please try again.\n");
            continue;
        }

        pthread_mutex_lock(&socket_mutex);
        if (send(sockfd, messageBuffer, strlen(messageBuffer), 0) < 0) {
            perror("Send failed");
            pthread_mutex_unlock(&socket_mutex);
            continue;
        }
        pthread_mutex_unlock(&socket_mutex);
    }
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
    printf("Enter username: ");
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
            return 0;
        }
    } else if (bytes_received == 0) {
        printf("Server disconnected.\n");
        return 0;
    } else {
        perror("Receive failed");
        return 0;
    }
}
