#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    errif(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1, "socket bind error");

    errif(listen(sockfd, SOMAXCONN) == -1, "socket listen error");

    int epfd = epoll_create1(0); // 创建epoll实例
    errif(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENTS],ev;
    bzero(&ev, sizeof(ev));
    bzro(&events, sizeof(events));
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = sockfd;
    setnonblocking(sockfd);
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev); //添加事件到epoll实例

    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        errif(nfds == -1, "epoll wait error");
        for(int i = 0; i < nfds; i++) {
            if(events[i].data.fd == sockfd) { //有新的连接
                struct sockaddr_in client_addr;
                bzero(&client_addr, sizeof(client_addr));
                socklen_t client_addr_len = sizeof(client_addr);

                int clnt_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_len);
                errif(clnt_sockfd == -1, "socket accept error");
                printf("new client %d! IP: %s Port:%d\n", clnt_sockfd,inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                bzero(&ev, sizeof(ev));
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = clnt_sockfd;
                setnonblocking(clnt_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sockfd, &ev); //添加事件到epoll实例
            } else if(events[i].events & EPOLLIN) { //可读事件
                char buf[RREAD_BUFFER];
                while(true) {
                    bzero(&buf, sizeof(buf));
                    ssize_t read_bytes = read(events[i].data.fd, buf, sizeof(buf));

                    if(read_bytes == -1 && errno == EINTR) {
                        printf("read error\n");
                        continue;
                    }
                    else if(read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) { //非阻塞IO，读完所有数据
                        printf("read all data\n");
                        break;
                    }
                    else if(read_bytes == 0) {
                        close(events[i].data.fd);
                        printf("client %d closed\n", events[i].data.fd);
                        break;
                    }
                    else {
                        printf("read %ld bytes: %s\n", read_bytes, buf);
                        write(events[i].data.fd, buf, read_bytes);
                    }
                    

                }
    } else {
        printf("other event\n");
    }
        }
    close(sockfd);
    return 0;


}
