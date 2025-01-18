#ifndef NETWORK_H
#define NETWORK_H

#include <netinet/in.h> // For sockaddr_in

#define PORT 8003 ///< Default port for the server and client communication.

/**
 * @brief Creates a TCP socket.
 * 
 * @return int The socket file descriptor on success, or -1 on failure.
 */
int createTCPSocket();

/**
 * @brief Binds a socket to a specific configuration (address and port).
 * 
 * @param sockfd The socket file descriptor.
 * @param config The configuration structure containing the address and port.
 */
void bindSocket(int sockfd, struct sockaddr_in *config);

/**
 * @brief Sets a socket to listen for incoming connections.
 * 
 * @param sockfd The socket file descriptor.
 * @param backlog The maximum number of pending connections.
 */
void setSocketToListen(int sockfd, int backlog);

/**
 * @brief Accepts a new client connection.
 * 
 * Waits for a client to connect to the server's listening socket.
 * 
 * @param serverSockfd The server's socket file descriptor.
 * @return int The client's socket file descriptor on success, or -1 on failure.
 */
int handleClientConnectionAttempt(int serverSockfd);

/**
 * @brief Connects a client socket to a server.
 * 
 * @param sockfd The client's socket file descriptor.
 * @param config The configuration structure containing the server's address and port.
 * @return int Returns 0 on success, or -1 on failure.
 */
int connectClientSocket(int sockfd, struct sockaddr_in *config);

/**
 * @brief Configures the server socket.
 * 
 * Initializes and returns a `struct sockaddr_in` configured for the server.
 * 
 * @param port The port number for the server to listen on.
 * @return struct sockaddr_in* Pointer to the server configuration structure.
 *         Caller is responsible for freeing the allocated memory.
 */
struct sockaddr_in *getServerConfig(int port);

/**
 * @brief Configures the client socket.
 * 
 * Initializes and returns a `struct sockaddr_in` configured for the client.
 * 
 * @param port The port number to connect to.
 * @param ip The server's IP address in dotted-decimal notation (e.g., "127.0.0.1").
 * @return struct sockaddr_in* Pointer to the client configuration structure.
 *         Caller is responsible for freeing the allocated memory.
 */
struct sockaddr_in *getClientConfig(int port, const char *ip);

#endif // NETWORK_H
