#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>

int initserver(int port) {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket");
        return -1;
    }
    int opt = 1;
    unsigned int len = sizeof(opt);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, len);
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(port);

    if(bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind failed !");
        close(listenfd);
        return -1;
    }
    if(listen(listenfd, 5) < 0) {
        perror("listen failed !");
        close(listenfd);
        return -1;
    }
    return listenfd;
}


int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }
    int listenfd = initserver(atoi(argv[1]));
    if(listenfd < 0) {
        printf("initserver failed\n");
        return -1;
    }
    printf("listenfd = %d\n", listenfd);

    //创建epoll句柄
    int epollfd = epoll_create(1);
    //初始化用于服务端监听的socket
    int listensock = initserver(atoi(argv[1]));
    //为服务端listensock准备读事件
    epoll_event ev;   //声明事件的结构体
    ev.data.fd = listensock;   //制定事件的自定义数据，会随着epoll_wait返回事件一起返回
    ev.events = EPOLLIN;       //监听读事件
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listensock, &ev); //把需要监视的socket添加到epoll句柄中
    epoll_event events[10]; //存放epoll返回事件
    //事件循环
    while(1) {
        //等待监视的socket事件发生
        int infds = epoll_wait(epollfd, events, 10, -1);
        //返回失败
        if(infds < 0) {
            perror("epoll_wait");
            return -1;
        }
        //超时
        if(infds == 0) {
            printf("epoll_wait timeout\n");
            continue;
        }

        //如果infds > 0,表示有活动事件,遍历所有活动事件
        for(int i = 0; i < infds; i++) {
            //如果listensock有读事件发生，表示有新的客户端连接
            if(events[i].data.fd == listensock) {
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int clientfd = accept(listensock, (struct sockaddr*)&clientaddr, &len);
                if(clientfd < 0) {
                    perror("accept");
                    continue;
                }

                printf("accept clientfd (socket = %d) ok.\n", clientfd);
                ev.data.fd = clientfd;
                ev.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
            }
            else {
                //处理客户端socket上的读事件，有报文发过来或者连接已断开
                char buf[1024];
                memset(buf, 0, sizeof(buf));
                if(recv(events[i].data.fd, buf, sizeof(buf), 0) <= 0) {
                    printf("clientfd (socket = %d) closed.\n", events[i].data.fd);
                    close(events[i].data.fd); //关闭socket
                } else {
                    //客户端有报文发过来
                    printf("recv(eventfd = d%):s% \n", events[i].data.fd, buf);
                    //如果发送缓冲区被填满，该函数会阻塞
                    send(events[i].data.fd, buf, strlen(buf), 0);
                }
            }
        }
    }
    return 0;
}