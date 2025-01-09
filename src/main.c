#include "client.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

void server(){
    struct sockaddr_in *config = getServerConfig(8080);
    int serverSockfd = createTCPSocket();
    bindSocket(serverSockfd, config);
    setSocketToListen(serverSockfd, 5);
    while(1){
        int clientSockfd = handleClientConnectionAttempt(serverSockfd);
        if(clientSockfd > 0){
            
        }
    }

}

int main(){
    server();
    return 0;
}

