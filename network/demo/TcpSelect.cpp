#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>

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

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("usage: ./tcpselect <port>\n", argv[0]);
        return -1;
    }
    int listenfd = initserver(8080);
    if (listenfd < 0) {
        printf("initserver failed\n");
        return -1;
    }

    //fd_set 本质是一个int数组，int[32], 大小为16字节(16*32=1024)，每个元素可以表示32个文件描述符
    fd_set readfds;   //需要监视的socket集合，大小为16字节的1024的bitmap
    FD_ZERO(&readfds);  //清空集合
    FD_SET(listenfd, &readfds); //将listenfd加入集合

    int maxfd = listenfd; //最大的文件描述符
    while(1) {
        //定义超时时间的结构体
        struct timeval timeout;
        timeout.tv_sec = 10;  //秒
        timeout.tv_usec = 0; //微秒

        //select函数会修改bitmap，所以需要临时变量
        fd_set tmpfds = readfds; //select函数中，会修改bitmap

        //第一个参数是bitmap的大小，第二个参数是需要监视的bitmap，第三个参数是写操作的bitmap，第四个参数是异常操作的bitmap（不需要填null），第五个参数是超时时间
        int infds = select(maxfd + 1, &tmpfds, NULL, NULL, &timeout);
        //三种返回情况：1.出错返回-1；2.超时返回0；3.有文件描述符发生变化返回大于0的值
        if(infds < 0) {
            perror("select failed");
            return -1;
        } else if(infds == 0) {
            printf("timeout\n");
            continue;
        } else {
            //遍历bitmap，找到有变化的文件描述符
            for(int i = 0; i <= maxfd; i++) {
                if(FD_ISSET(i, &tmpfds) == 0) {
                    continue; //当前文件描述符没有发生变化
                }
                if(i == listenfd) {
                    //有新的连接
                    struct sockaddr_in clientaddr;
                    socklen_t len = sizeof(clientaddr);
                    int connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &len);
                    if(connfd < 0) {
                        perror("accept failed");
                        continue;
                    }
                    FD_SET(connfd, &readfds); //将新的连接加入集合,将bitmap的相应位置1
                    if(connfd > maxfd) {
                        maxfd = connfd; //更新最大文件描述符
                    }
                } else {
                    //有数据可读
                    char buf[1024];
                    memset(buf, 0, sizeof(buf));
                    if(recv(i, buf, sizeof(buf), 0) <= 0) {
                        //客户端连接断开或者出错
                        perror("read failed");
                        close(i);
                        FD_CLR(i, &readfds); //将bitmap的相应位置0
                        if (i == maxfd)      //重新计算最大文件描述符
                        {
                            for(int ii = maxfd; ii > 0; ii--) {
                                if(FD_ISSET(ii, &readfds)) {
                                    maxfd = ii;
                                    break;
                                }
                            }
                        }    
                    } else {
                        //客户端有报文发送过来了
                        printf("recv:(eventfd = %d) %s\n",i, buf);
                        //接受到的报文发送回去
                        if(send(i, buf, strlen(buf), 0) < 0) {
                            perror("send failed");
                            close(i);
                            FD_CLR(i, &readfds); //将bitmap的相应位置0
                        }
                        }
                }

            }
    
        }
    }
    return 0;

}