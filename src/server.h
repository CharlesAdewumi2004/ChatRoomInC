#include <netinet/in.h>
struct sockaddr_in *getServerConfig(int port);
int createTCPSocket();
void bindSocket(int serverSockfd, struct sockaddr_in *config);
void setSocketToListen(int serverSockfd, int backLog);
int handleClientConnectionAttempt(int serverSockfd);