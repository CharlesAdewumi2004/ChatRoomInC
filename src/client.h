#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h> // For mutex

#define MAX_USERNAME_LENGTH 50 ///< Maximum length for a username.

/**
 * @brief Mutex for thread-safe socket operations.
 * 
 * Ensures that multiple threads (e.g., send and receive threads) do not
 * simultaneously access the socket, avoiding race conditions.
 */
extern pthread_mutex_t socket_mutex;

/**
 * @brief Handles server communication for the client.
 * 
 * Starts threads for sending and receiving messages, and manages client-server interaction.
 * 
 * @param sockfd The socket file descriptor for the client-server connection.
 */
void handleServer(int sockfd);

/**
 * @brief Sends the username to the server and validates the response.
 * 
 * Prompts the user to enter a username, sends it to the server, and waits for validation.
 * 
 * @param sockfd The socket file descriptor for the client-server connection.
 * @return int Returns 1 if the username is valid, or 0 if invalid or rejected by the server.
 */
int setUsername(int sockfd);

/**
 * @brief Receives messages from the server in a loop.
 * 
 * Continuously listens for messages from the server and displays them to the user.
 * 
 * @param arg Pointer to the socket file descriptor.
 * @return void* Always returns NULL when the thread exits.
 */
void *recvMessage(void *arg);

/**
 * @brief Sends messages to the server in a loop.
 * 
 * Continuously reads user input and sends it to the server.
 * 
 * @param arg Pointer to the socket file descriptor.
 * @return void* Always returns NULL when the thread exits.
 */
void *sendMessage(void *arg);

#endif // CLIENT_H
