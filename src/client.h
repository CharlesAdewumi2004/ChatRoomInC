#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h> // Required for thread-related declarations

// Constants
#define MAX_USERNAME_LENGTH 50
#define MAX_RETRIES 3

// Mutex for thread-safe operations
extern pthread_mutex_t socket_mutex;

// Function Declarations

/**
 * @brief Handles client interaction with the server.
 *
 * @param sockfd Socket file descriptor.
 */
void handleServer(int sockfd);

/**
 * @brief Sends the username to the server and validates the response.
 *
 * @param sockfd Socket file descriptor.
 * @return int Returns 1 if the username is valid, 0 otherwise.
 */
int setUsername(int sockfd);

/**
 * @brief Receives messages from the server in a loop.
 *
 * @param arg Pointer to the socket file descriptor.
 * @return void* Returns NULL on thread exit.
 */
void *recvMessage(void *arg);

/**
 * @brief Sends messages to the server in a loop.
 *
 * @param arg Pointer to the socket file descriptor.
 * @return void* Returns NULL on thread exit.
 */
void *sendMessage(void *arg);

#endif // CLIENT_H
