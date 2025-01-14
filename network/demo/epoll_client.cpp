#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

int setnonblocking(int fd) {
    int flags;
    if((flags = fcntl(fd, F_GETFL, 0)) < 0) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        return -1;
    }

    int sockfd;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    char buf[1024];
    if(sockfd = socket(AF_INET, SOCK_STREAM, 0) < 0 ) {
        printf("socket() error: %s\n");
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("connect() error: %s\n", strerror(errno));
        return -1;
    }
    printf("connect success\n");
    for(int ii = 0; ii < 20000; ii++) {
        memset(buf, 0, sizeof(buf));
        printf("please input:");
        scanf("%s", buf);
        if(send(sockfd, buf, strlen(buf), 0) < 0) {
            printf("send() error: %s\n");
            close(sockfd);
            return -1;
        }
        memset(buf, 0, sizeof(buf));
        if(recv(sockfd, buf, sizeof(buf), 0) < 0) {
            printf("recv() error: %s\n");
            close(sockfd);
            return -1;
        }
        printf("recv: %s\n", buf);
    }
}