#include "client.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;

int setUsername(int sockfd) {
    char username[MAX_USERNAME_LENGTH];
    char response[2];

    printf("Enter username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    if (send(sockfd, username, strlen(username), 0) < 0) {
        perror("Failed to send username");
        return 0;
    }

    int bytes_received = recv(sockfd, response, sizeof(response) - 1, 0);
    if (bytes_received > 0) {
        response[bytes_received] = '\0';
        return strcmp(response, "1") == 0;
    }
    return 0;
}

void *recvMessage(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[256];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("%s\n", buffer);
        } else {
            break;
        }
    }

    pthread_exit(NULL);
}

void *sendMessage(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[256];

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0) continue;

        if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("Failed to send message");
            break;
        }
    }

    pthread_exit(NULL);
}

void handleServer(int sockfd) {
    pthread_t send_thread, recv_thread;

    pthread_create(&recv_thread, NULL, recvMessage, &sockfd);
    pthread_create(&send_thread, NULL, sendMessage, &sockfd);

    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);
}
