#include "network.h"
#include <netinet/in.h>
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
    }

}

void client(){
    struct sockaddr_in *config = getClientConfig(8080, "127.0.0.1");
    int clientSockfd = createTCPSocket();
    if(connectClientSocket(clientSockfd, config) == -1){
        return;
    }
}

int main(){
    int choice;
    printf("Enter 1 to run a server and 2 to run a client: \t");
    scanf("%d", &choice);
    if(choice == 1){
        server();
    }else{
        client();
    }
    return 0;
}

