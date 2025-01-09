#include "server.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

/**
 * @brief handles attempted connection to the server
 *
 * @param serverSockfd server socket file descriptor
 * @return client socket file descriptor or -1 on failure.
 */
int handleClientConnectionAttempt(int serverSockfd){
    while(1){
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        int* clientSockfd = malloc(sizeof(int));
        *clientSockfd = accept(serverSockfd, (struct sockaddr*)&client_address, &client_len);
        if (*clientSockfd < 0) {
            perror("Accept failed");
            free(clientSockfd);
            continue;
        }else{
            printf("Connection accepted from %s:%d\n",inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
            return *clientSockfd;
        }
    }
    return -1;
}

/**
 * @brief sets socket to listen for connections
 *
 * @param serverSockfd server socket file descriptor
 * @param backLog maximum number of queued connections 
 * @return void
 */
void setSocketToListen(int serverSockfd, int backLog){
    if(listen(serverSockfd, backLog) != 0){
        perror("error setting socket to listen");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief binds socket
 *
 * @param serverSockfd server socket file descriptor
 * @param config configureation of server(port, IP address, ...)
 * @return void
 */
void bindSocket(int serverSockfd, struct sockaddr_in *config){
    if (bind(serverSockfd, (struct sockaddr*)config, sizeof(*config)) < 0) {
        perror("Bind failed");
        close(serverSockfd);
        exit(EXIT_FAILURE);
    }else{
        printf("Successfully binded socket");
    }
}

/**
 * @brief creates TCP socket 
 *
 * @return socket file descriptor 
 */
int createTCPSocket(){
    int serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSockfd < 0){
        perror("Failed to create the socket");
        exit(EXIT_FAILURE);
    }
    return serverSockfd;
}

/**
 * @brief sets the configureation for the sokcket
 *
 * @param port port number
 * @return socket configureation
 */
struct sockaddr_in *getServerConfig(int port) {
    static struct sockaddr_in config; // Static variable persists across function calls
    static int initialized = 0;

    if (!initialized) {
        memset(&config, 0, sizeof(config)); // Clear the structure
        config.sin_family = AF_INET;       // Use IPv4
        config.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces
        config.sin_port = htons(port);     // Convert port to network byte order
        initialized = 1;
    }

    return &config; // Return the address of the static struct
}



