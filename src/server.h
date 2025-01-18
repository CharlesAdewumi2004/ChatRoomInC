#ifndef SERVER_H
#define SERVER_H

#include <pthread.h> // For thread-related types and functions

#define MAX_CLIENTS 50 ///< Maximum number of clients that can connect.
#define MAX_USERNAME_LENGTH 20 ///< Maximum length of a username.

/**
 * @struct ClientInfo
 * @brief Stores information about a connected client.
 */
typedef struct {
    int sockfd; ///< Socket file descriptor for the client.
    char username[MAX_USERNAME_LENGTH]; ///< Username of the client.
} ClientInfo;

/**
 * @brief Initializes the client list.
 * 
 * Sets all `sockfd` values to -1 and clears all usernames.
 */
void initializeClients();

/**
 * @brief Validates and sets a username for a client.
 * 
 * @param sockfd The client's socket file descriptor.
 * @param username The username to validate and assign.
 * @return int Returns 1 if the username is successfully set, or 0 if invalid or already taken.
 */
int validateAndSetUsername(int sockfd, const char *username);

/**
 * @brief Handles multiple attempts to set a username for a client.
 * 
 * @param sockfd The client's socket file descriptor.
 * @return int Returns 1 if a username is successfully set, or 0 if the client disconnects.
 */
int attemptToSetUsername(int sockfd);

/**
 * @brief Removes a client from the client list.
 * 
 * Marks the client's slot as unused and clears the associated username.
 * 
 * @param sockfd The client's socket file descriptor.
 */
void removeClient(int sockfd);

/**
 * @brief Broadcasts a message to all connected clients except the sender.
 * 
 * @param message The message to broadcast.
 * @param sender_sockfd The socket file descriptor of the sender.
 */
void broadcastMessage(const char *message, int sender_sockfd);

/**
 * @brief Handles a client's connection.
 * 
 * Manages username setup and message handling for a connected client.
 * 
 * @param arg Pointer to the client's socket file descriptor.
 * @return void* Always returns NULL.
 */
void *handleClient(void *arg);

#endif // SERVER_H

