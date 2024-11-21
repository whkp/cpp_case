#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "util.h"

#define BUFFER_SIZE 1024

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    errif(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "connect error");

    while (true) {
        char buffer[BUFFER_SIZE];
        bzero(buffer, BUFFER_SIZE);
        scanf("%s", buffer);
        ssize_t n = write(sockfd, buffer, sizeof(buffer));
        if (n == -1) {
            printf("write error, socket already disconnected\n");
            break;
        }
        bzero(buffer, BUFFER_SIZE);
        ssize_t m = read(sockfd, buffer, sizeof(buffer));
        if (m == -1) {
            close(sockfd);
            errif(true, "read error");
        } else if (m == 0) {
            printf("server close\n");
            break;
        } else {
            printf("read %ld bytes from server: %s\n", m, buffer);
        }
    }
    close (sockfd);
    return 0;
}