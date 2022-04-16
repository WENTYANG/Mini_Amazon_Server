#ifndef _SOCKET_H
#define _SOCKET_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

using namespace std;

int createServerSocket(const string & portNum);
int serverAcceptConnection(int serverFd, string & clientIp);
int clientRequestConnection(const string & hostName, const string & portNum);
int getPortNum(int socketFd);
void sendMsg(int socket_fd, const void * buf, int len);
void recvMsg(int socket_fd, void * buf, int & len);

#endif