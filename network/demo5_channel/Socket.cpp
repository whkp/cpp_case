#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

Socket::Socket() : fd(-1) {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
}
Socket::Socket(int fd) : fd(fd) {
    errif(fd == -1, "socket create error");
}

Socket::~Socket() {
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress &addr) {
    //::bind调用系统bind函数，而不是Socket类的bind函数
    errif(::bind(fd, (struct sockaddr*)&addr.addr, addr.addr_len) == -1, "bind error");
}

void Socket::listen() {
    errif(::listen(fd, SOMAXCONN) == -1, "listen error");
}

void Socket::setNonBlock() {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}
int Socket::accept(InetAddress &addr) {
    int clientfd = ::accept(fd, (struct sockaddr*)&addr.addr, &addr.addr_len);
    errif(clientfd == -1, "accept error");
    return clientfd;
}
int Socket::getFd() {
    return fd;
}