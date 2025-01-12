#ifndef SERVER_H
#define SERVER_H

#define MAX_CLIENTS 50

typedef struct {
    int sockfd;          // Socket file descriptor
    char username[20];   // Username
} ClientInfo;

void initializeClients();
int validateAndSetUsername(int sockfd, const char *username);
int attemptToSetUsername(int sockfd);
const char* findUsernameBySockfd(int sockfd);
void removeClient(int sockfd);
void handleClient(int sockfd);

#endif
