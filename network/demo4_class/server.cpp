#include "Epoll.h"
#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <errno.h>

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setNonBlocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK); // 设置为非阻塞
}

void handleReadEvent(int fd) {
    char buf[READ_BUFFER];
    while (true) {
        memset(buf, 0, READ_BUFFER);
        ssize_t n = read(fd, buf, READ_BUFFER);
        if (n == -1 && (errno != EAGAIN || errno != EWOULDBLOCK)) {//非阻塞读取，读取完毕后返回-1
            printf("finish reading once\n");
            return;
        } else if (n == 0) { //客户端关闭
            printf("client close\n");
            close(fd); //关闭文件描述符自动从epoll中移除
            return;
        } else if(n == -1 && errno == EINTR) { //客户端正常中断、信号中断
            printf("continue reading\n");
            continue;
        } else {
            printf("read %ld bytes from client fd %d : %s\n", n,fd, buf);
            write(fd, buf, sizeof(buf)); //回显
        }
    }
}

int main() {
    Socket *serv_sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(addr);
    serv_sock->listen();
    Epoll *epoll = new Epoll();
    epoll->addFd(serv_sock->getFd(), EPOLLIN);
    while (true) {
        std::vector<epoll_event> events = epoll->poll();
        int nfds = events.size();
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == serv_sock->getFd()) {
                InetAddress *client_addr = new InetAddress();
                Socket *client_sock = new Socket(serv_sock->accept(client_addr));
                printf("new client: %s:%d\n", inet_ntoa(client_addr->addr.sin_addr), ntohs(client_addr->addr.sin_port));
                clnt_sock->setNonBlock();
                epoll->addFd(clnt_sock->getFd(), EPOLLIN | EPOLLET);
            } else if(events[i].events & EPOLLIN) { //可读事件
                handleReadEvent(events[i].data.fd);
            } else {
                printf("something else event\n");
            }
            }
        }
    delete serv_sock;
    delete addr;
    return 0;
}
