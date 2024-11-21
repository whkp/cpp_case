#include "Socket.h"
#include "InetAddress.h"
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "util.h"


Socket::Socket() : fd(-1) {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error!");
}

Socket::Socket(int fd) : fd(fd) {
    errif(fd == -1, "socket create error!");
}

Socket::~Socket() {
    if(fd != -1) {
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress &addr) {
    errif(bind(fd, (sockaddr *) &addr, sizeof(InetAddress)) == -1, "bind error!");
}

void Socket::listen() {
    errif(listen(fd, SOMAXCONN) == -1, "listen error!");
}

void Socket::setNonBlock() {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK); // 设置为非阻塞
}

int Socket::accept(InetAddress &addr) {
    int connfd = accept(fd, (sockaddr *) &addr, &addr.addr_len);
    errif(connfd == -1, "accept error!");
    return connfd;
}

int Socket::getfd() {
    return fd;
}