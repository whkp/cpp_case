#include "Socket.h"
#include "util.h"
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

InetAddress::InetAddress() {
    //初始化地址结构体
    bzero(&addr_, sizeof(addr_));
}

InetAddress::InetAddress(const char* ip, uint16_t port) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET; //IPv4
    addr_.sin_port = htons(port); //主机序转网络序
    addr_.sin_addr.s_addr = inet_addr(ip); //点分十进制转网络序
}

InetAddress::~InetAddress() {
}

void InetAddress::setInetAddr(sockaddr_in addr) {
    addr_ = addr;
}

sockaddr_in InetAddress::getInetAddr() const {
    return addr_;
}

char* InetAddress::getIp() const {
    return inet_ntoa(addr_.sin_addr);  //  将网络序的IP地址转换为点分十进制字符串并返回
}

uint16_t InetAddress::getPort() const {
    return ntohs(addr_.sin_port); //将网络序的端口号转换为主机序并返回
}

Socket::Socket() : fd_(-1) {
    //创建套接字
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    ErrorIf(fd_ == -1, "socket create error");
}

Socket::Socket(int fd) : fd_(fd) {
}

Socket::~Socket() {
    if(fd_ != -1) {
        close(fd_);
        fd_ = -1; //防止重复关闭
    }
}

void Socket::Bind(InetAddress* addr) {
    int ret = ::bind(fd_, (struct sockaddr*)&(addr->getInetAddr()), sizeof(addr->getInetAddr()));
    ErrorIf(ret == -1, "socket bind error");
}

void Socket::Listen() {
    ErrorIf(::listen(fd_, SOMAXCONN) == -1, "socket listen error");
}

int Socket::Accept(InetAddress* addr) {
    //阻塞和非阻塞模式代码不一样
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientFd = ::accept(fd_, (struct sockaddr*)&clientAddr, &clientAddrLen);
    ErrorIf(clientFd == -1, "socket accept error");
    addr->setInetAddr(clientAddr);
    return clientFd;
}

void Socket::Connect(InetAddress* addr) {
    //阻塞和非阻塞模式代码不一样
    struct sockaddr_in serverAddr = addr->getInetAddr();
    ErrorIf(::connect(fd_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1, "socket connect error");
}
void Socket::Connect(const char* ip, uint16_t port) {
    InetAddress addr(ip, port);
    Connect(&addr);
}

void Socket::setNonBlocking() {
    //获取fd的flags
    int flags = fcntl(fd_, F_GETFL, 0);
    ErrorIf(flags == -1, "socket get flags error");
    //设置为非阻塞
    int ret = fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
    ErrorIf(ret == -1, "socket set nonblocking error");
}
bool Socket::isNonBlocking() {
    return (fcntl(fd_, F_GETFL, 0) & O_NONBLOCK) != 0;
}
int Socket::getFd() {
    return fd_;
}

void Socket::setFd(int fd) {
    fd_ = fd;
}