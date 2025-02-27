#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "util.h"
#include "Channel.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

// 设置文件描述符为非阻塞模式
void setnonbblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}
// 处理读事件
void handleReadEvent(int fd) {
    char buf[READ_BUFFER];
    while(true) {
        bzero(&buf,sizeof(buf));
        ssize_t n = read(fd, buf, sizeof(buf));
        if(n > 0) {
            printf("message from client fd %d: %s\n", fd, buf);
            write(fd, buf, sizeof(buf));
        } else if(n == -1 && errno == EINTR) { //客户端正常中断
            printf("continue read\n");
            continue;
        } else if(n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) { //客户端非阻塞读，没有数据可读
            printf("read all data\n");
            break;
        } else if(n == 0) { // 客户端断开连接
            printf("client fd %d disconnected\n", fd);
            close(fd); //关闭客户端连接，自动从epoll中移除
            break;
        }
    }
}

int main() {
    // 创建服务器套接字
    Socket *serv_sock = new Socket();
    // 创建服务器地址
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    // 绑定服务器套接字到服务器地址
    serv_sock -> bind(*serv_addr);
    // 监听服务器套接字
    serv_sock -> listen();
    // 创建epoll对象
    Epoll *epoll = new Epoll();
    // 设置服务器套接字为非阻塞模式
    serv_sock -> setNonBlocking();
    // 创建服务器通道
    Channel *serv_channel = new Channel(epoll, serv_sock -> getFd());
    // 启用服务器通道的读事件
    serv_channel -> enableReading();
    while(true) {
        // 获取活跃的通道
        std::vector<Channel*> activeChannels = epoll -> poll();
        for(int i = 0; i < activeChannels.size(); i++) {
            int fd = channel -> getFd();
            if(fd == serv_sock -> getFd()) { //新的客户端连接
                InetAddress *client_addr = new InetAddress();
                Socket *client_sock = new Socket(serv_sock -> accept(*client_addr));
                printf("new client fd %d connected\n", client_sock -> getFd());
                client_sock -> setNonBlocking();
                Channel *client_channel = new Channel(epoll, client_sock -> getFd());
                client_channel -> enableReading();
            } else if(activeChannels[i] -> getRevents() & EPOLLIN) { //客户端有数据可读
                handleReadEvent(fd);
            } else {
                printf("something else happened\n", fd);
            }
        }
    }
    // 释放服务器套接字和服务器地址
    delete serv_sock;
    delete serv_addr;
    return 0;
}