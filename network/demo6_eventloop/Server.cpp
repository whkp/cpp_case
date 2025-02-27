#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include <functional>
#include <string.h>
#include <unistd.h>

#define READ_BUFFER 1024

Server::Server(EventLoop* loop) : loop(loop) {
    // 创建服务器套接字
    serv_sock = new Socket();
    // 创建服务器地址
    serv_addr = new InetAddress("127.0.0.1", 8888);
    // 绑定服务器地址到服务器套接字
    serv_sock->bind(*serv_addr);
    // 监听服务器套接字
    serv_sock->listen();
    // 设置服务器套接字为非阻塞模式
    serv_sock->setNonBlock();
    
    Channel* serv_channel = new Channel(loop, serv_sock->getFd());
    std::function< void()> cb = std::bind(&Server::newConnection, this);
    serv_channel->setCallback(cb);
    serv_channel->enableReading();
}

Server::~Server() {
}

void Server::handleReadEvent() {
    char buf[READ_BUFFER];
    while(true) { //由于使用非阻塞IO,读取客户端buffer,一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0) {
             printf("message from client fd %d: %s\n", sockfd, buf);
             write(sockfd, buf, sizeof(buf));
        } else if(bytes_read == -1 && errno == EINTR) { //客户端正常中断,继续读取
                printf("continue read\n");
                continue;
        } else if(bytes_read == -1 && (errno == EAGAIN) || errno == EWOULDBLOCK) { //非阻塞IO,读取完毕
                printf("finish read once, errno: %d\n", errno);
                break;
        } else if(bytes_read == 0) { //客户端关闭
                printf("client closed\n");
                close(sockfd);
                break;
        }
    }
}

void Server::newConnection(Socket* serv_sock) {
    InetAddress* cli_addr = new InetAddress();
    Socket* clnt_sock = new Socket(serv_sock->accept(*cli_addr));
    printf("new client fd %d! IP: %s Port: %d\n",clnt_sock->getFd(), cli_addr->addr.sin_addr, ntohs(cli_addr->addr.sin_port));
    clnt_sock->setNonBlock();
    Channel* clnt_channel = new Channel(loop, clnt_sock->getFd());
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this);
    clnt_channel->setCallback(cb);
    clnt_channel->enableReading();
}