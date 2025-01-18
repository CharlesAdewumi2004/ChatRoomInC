#include "network.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * @brief Creates a TCP socket.
 * 
 * @return int The socket file descriptor, or -1 on error.
 */
int createTCPSocket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
    }
    return sockfd;
}

/**
 * @brief Binds a socket to a specific configuration.
 * 
 * @param sockfd The socket file descriptor.
 * @param config The socket configuration (address, port, etc.).
 */
void bindSocket(int sockfd, struct sockaddr_in *config) {
    if (bind(sockfd, (struct sockaddr *)config, sizeof(*config)) < 0) {
        perror("Failed to bind socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Sets a socket to listen for incoming connections.
 * 
 * @param sockfd The socket file descriptor.
 * @param backlog The maximum number of pending connections.
 */
void setSocketToListen(int sockfd, int backlog) {
    if (listen(sockfd, backlog) < 0) {
        perror("Failed to listen on socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Accepts a new client connection.
 * 
 * @param serverSockfd The server's socket file descriptor.
 * @return int The client's socket file descriptor, or -1 on error.
 */
int handleClientConnectionAttempt(int serverSockfd) {
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);

    int clientSockfd = accept(serverSockfd, (struct sockaddr *)&client_addr, &addrlen);
    if (clientSockfd < 0) {
        perror("Failed to accept client connection");
    } else {
        printf("New client connected: %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
    }
    return clientSockfd;
}

/**
 * @brief Connects a client socket to a server.
 * 
 * @param sockfd The client socket file descriptor.
 * @param config The server configuration (address, port, etc.).
 * @return int 0 on success, -1 on failure.
 */
int connectClientSocket(int sockfd, struct sockaddr_in *config) {
    if (connect(sockfd, (struct sockaddr *)config, sizeof(*config)) < 0) {
        perror("Failed to connect to server");
        return -1;
    }
    printf("Connected to server.\n");
    return 0;
}

/**
 * @brief Configures the server socket.
 * 
 * @param port The port number for the server to listen on.
 * @return struct sockaddr_in* The server configuration.
 */
struct sockaddr_in *getServerConfig(int port) {
    struct sockaddr_in *config = malloc(sizeof(struct sockaddr_in));
    if (config == NULL) {
        perror("Malloc failed for server config");
        exit(EXIT_FAILURE);
    }

    memset(config, 0, sizeof(struct sockaddr_in));
    config->sin_family = AF_INET;
    config->sin_addr.s_addr = INADDR_ANY;
    config->sin_port = htons(port);

    return config;
}

/**
 * @brief Configures the client socket.
 * 
 * @param port The port number to connect to.
 * @param ip The server's IP address.
 * @return struct sockaddr_in* The client configuration.
 */
struct sockaddr_in *getClientConfig(int port, const char *ip) {
    struct sockaddr_in *config = malloc(sizeof(struct sockaddr_in));
    if (config == NULL) {
        perror("Malloc failed for client config");
        exit(EXIT_FAILURE);
    }

    memset(config, 0, sizeof(struct sockaddr_in));
    config->sin_family = AF_INET;
    config->sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &config->sin_addr) <= 0) {
        perror("Invalid IP address");
        free(config);
        exit(EXIT_FAILURE);
    }

    return config;
}
