#ifndef SERVER_H
#define SERVER_H

#include <pthread.h> // For thread-related types and functions

#define MAX_CLIENTS 50 // Maximum number of connected clients

/**
 * @brief Structure to store client information.
 */
typedef struct {
    int sockfd;           // Socket file descriptor
    char username[20];    // Username
} ClientInfo;

// **Client Management Functions**

/**
 * @brief Initializes the client list.
 *        Sets all `sockfd` to -1 and clears usernames.
 */
void initializeClients();

/**
 * @brief Validates and sets the username for a client.
 *
 * @param sockfd The client's socket file descriptor.
 * @param username The username to validate and set.
 * @return int Returns 1 if the username is successfully set, 0 otherwise.
 */
int validateAndSetUsername(int sockfd, const char *username);

/**
 * @brief Handles multiple attempts to set a username for a client.
 *
 * @param sockfd The client's socket file descriptor.
 * @return int Returns 1 if a username is successfully set, 0 otherwise.
 */
int attemptToSetUsername(int sockfd);

/**
 * @brief Finds a client's username by their socket file descriptor.
 *
 * @param sockfd The socket file descriptor of the client.
 * @return const char* Returns the username if found, or NULL otherwise.
 */
const char* findUsernameBySockfd(int sockfd);

/**
 * @brief Removes a client from the client list.
 *
 * @param sockfd The socket file descriptor of the client to remove.
 */
void removeClient(int sockfd);

// **Client Connection and Message Handling Functions**

/**
 * @brief Handles client communication with the server.
 *
 * @param arg Pointer to the socket file descriptor of the client.
 * @return void* Always returns NULL.
 */
void *handleClientServer(void *arg);

/**
 * @brief Broadcasts a message to all connected clients except the sender.
 *
 * @param message The message to broadcast.
 * @param sender_sockfd The socket file descriptor of the sender.
 */
void broadcastMessage(const char *message, int sender_sockfd);

/**
 * @brief Receives messages from a client and processes them.
 *
 * @param sockfd The socket file descriptor of the client.
 */
void recvMessageServer(int sockfd);

#endif // SERVER_H


