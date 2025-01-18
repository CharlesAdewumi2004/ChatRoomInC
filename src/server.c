#include "server.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

ClientInfo clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void initializeClients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].sockfd = -1;
        memset(clients[i].username, 0, sizeof(clients[i].username));
    }
}

int validateAndSetUsername(int sockfd, const char *username) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd != -1 && strcmp(clients[i].username, username) == 0) {
            pthread_mutex_unlock(&clients_mutex);
            return 0;
        }
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd == -1) {
            clients[i].sockfd = sockfd;
            strncpy(clients[i].username, username, sizeof(clients[i].username) - 1);
            pthread_mutex_unlock(&clients_mutex);
            return 1;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return 0;
}

int attemptToSetUsername(int sockfd) {
    char username[100];
    char response[2];

    while (1) {
        int bytes_received = recv(sockfd, username, sizeof(username) - 1, 0);
        if (bytes_received > 0) {
            username[bytes_received] = '\0';
            if (validateAndSetUsername(sockfd, username)) {
                strcpy(response, "1");
                send(sockfd, response, strlen(response), 0);
                return 1;
            } else {
                strcpy(response, "0");
                send(sockfd, response, strlen(response), 0);
            }
        } else {
            close(sockfd);
            return 0;
        }
    }
}

void removeClient(int sockfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd == sockfd) {
            clients[i].sockfd = -1;
            memset(clients[i].username, 0, sizeof(clients[i].username));
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void broadcastMessage(const char *message, int sender_sockfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd != -1 && clients[i].sockfd != sender_sockfd) {
            send(clients[i].sockfd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handleClient(void *arg) {
    int sockfd = *(int *)arg;
    free(arg);

    if (!attemptToSetUsername(sockfd)) {
        close(sockfd);
        return NULL;
    }

    char buffer[256];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            for(int i = 0; i < MAX_CLIENTS; i++){
                if (clients[i].sockfd == sockfd) {
                    strcat(buffer, "||sent by: ");
                    strcat(buffer, clients[i].username);
                    broadcastMessage(buffer, sockfd);
                    break;
                }
            }
        } else {
            close(sockfd);
            removeClient(sockfd);
            break;
        }
    }

    return NULL;
}
