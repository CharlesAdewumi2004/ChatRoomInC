#include <netinet/in.h>
struct sockaddr_in *getServerConfig(int port);
struct sockaddr_in *getClientConfig(int port, char *ip);
int createTCPSocket();
void bindSocket(int sockfd, struct sockaddr_in *config);
void setSocketToListen(int sockfd, int backLog);
int handleClientConnectionAttempt(int sockfd);
int connectClientSocket(int sockfd, struct sockaddr_in *config);


//server specific stuff
void handleClient(int sockfd);

//clinet specific stuff
void handleServer(int sockfd);