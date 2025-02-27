#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "util.h"

define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv.addr.sin_port = htons(8888);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    errif(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");
    while(true) {
        char buf[BUFFER_SIZE];
        bzero(buf, BUFFER_SIZE);
        scanf("%s", buf);
        ssize_t write_bytes = write(sockfd, buf, sizeof(buf));
        errif(write_bytes == -1, "socket write error");
        bzero(&buf, BUFFER_SIZE);
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf));
        if(read_bytes == 0) {
            printf("server socket disconnected\n");
        } else if(read_bytes == -1) {
            close(sockfd);
            printf("socket read error\n");
        } else {
            printf("read message from server: %s\n", buf);
        }
    }
    close(sockfd);
    return 0;
}