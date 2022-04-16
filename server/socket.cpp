#include "socket.h"

#include "exception.h"

/*
    create a socket run as a server. Listen to the portNum.
    return thee server socket. If it fails, it will throw exception.
*/
int createServerSocket(const string& portNum) {
    const char* hostname = NULL;  // default 0.0.0.0
    struct addrinfo host_info;
    struct addrinfo* host_info_list;
    int status;
    int socket_fd;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    // get the socket address(ip + port number)
    status =
        getaddrinfo(hostname, portNum.c_str(), &host_info, &host_info_list);
    if (status != 0) {
        throw MyException("Error: cannot get address info for host\n");
    }
    if (portNum.empty()) {  // empty port number makes programe find a random
                            // port number by itself
        struct sockaddr_in* addr_in =
            (struct sockaddr_in*)(host_info_list->ai_addr);
        addr_in->sin_port = 0;
    }

    // create socket
    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
        throw MyException("Error: cannot create socket");
    }

    // set socket bind to certain address
    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                        sizeof(int));  //设置socket状态
    status =
        bind(socket_fd, host_info_list->ai_addr,
             host_info_list->ai_addrlen);  // bind a socket address to a socket
    if (status == -1) {
        throw MyException("Error:cannot bind socket");
    }

    // listen to port and wait for connection
    status = listen(socket_fd, 100);
    if (status == -1) {
        throw MyException("Error:cannot listen on socket");
    }

    freeaddrinfo(host_info_list);
    return socket_fd;
}

/*
    socket serverFd accept the request from clients.
    Parse client's IP address from its socket adress, save it in clientIp.
    return the connect socket. If it fails, it will throw exception.
*/
int serverAcceptConnection(int serverFd, string& clientIp) {
    struct sockaddr_storage socket_addr;  // store client socket address
    socklen_t socket_addr_len = sizeof(socket_addr);

    // accepty connection
    int client_connection_fd =
        accept(serverFd, (struct sockaddr*)&socket_addr,
               &socket_addr_len);  // block until connection build
    if (client_connection_fd == -1) {
        throw MyException("Error: cannot accept connection on socket\n");
    }

    // get client ip from its socket address
    struct sockaddr_in* addr = (struct sockaddr_in*)&socket_addr;
    clientIp = inet_ntoa(addr->sin_addr);

    return client_connection_fd;
}

/*
    Work as a client, create a socket to request connection to server, based on
   hostName and portNum. return the connection socket
*/
int clientRequestConnection(const string& hostName, const string& portNum) {
    int status;
    struct addrinfo host_info;
    struct addrinfo* host_info_list;

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(hostName.c_str(), portNum.c_str(), &host_info,
                         &host_info_list);
    if (status != 0) {
        throw MyException("Error: cannot get address info for host\n");
    }

    int client_fd =
        socket(host_info_list->ai_family, host_info_list->ai_socktype,
               host_info_list->ai_protocol);
    if (client_fd == -1) {
        throw MyException("Error: cannot create socket\n");
    }

    status = connect(client_fd, host_info_list->ai_addr,
                     host_info_list->ai_addrlen);  //建立连接
    if (status == -1) {
        throw MyException("Error: cannot connect to socket\n");
    }

    freeaddrinfo(host_info_list);
    return client_fd;
}

/*
    return the port number of socket socketFd.
*/
int getPortNum(int socketFd) {
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(socketFd, (struct sockaddr*)&sin, &len) == -1) {
        throw MyException("Error: cannot getsockname\n");
    }
    return ntohs(sin.sin_port);
}

/*
  send msg to the given socket. If it fails, it will throw exception
  and close socket.
*/
void sendMsg(int socket_fd, const void* buf, int len) {
    if (send(socket_fd, buf, len, 0) < 0) {
        close(socket_fd);
        throw MyException("fail to send msg.");
    }
}

/*
  receive msg from the given socket. If it fails, it will throw exception
  and close socket.
*/
void recvMsg(int socket_fd, void* buf, int& len) {
    len = recv(socket_fd, buf, len, 0);
    if (len <= 0) {
        close(socket_fd);
        std::cerr << "len: " << len << endl;
        std::cerr << "errno: " << errno << endl;
        throw MyException("fail to accept msg.");
    }
}