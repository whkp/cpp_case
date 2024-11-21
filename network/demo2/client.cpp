#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

void errif(bool condition, const char *errmsg) {
    if(condition) {
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    ////设置客户端地址、IP和端口
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    //bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)); 客户端不进行bind操作

    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)), "connect error");    
    
    while (true) {
        char buf[1024];
        bzero(&buf, sizeof(buf));
        scanf("%s", buf); //从标准输入读取数据
        ssize_t write_bytes = write(sockfd, buf, strlen(buf)); //将缓冲区数据写入服务器socket
        if (write_bytes == -1) {
            printf("socket already disconnected, cannot write any more");
            break;
        }
        bzero(&buf, sizeof(buf));
        ssize_t read_bytes = read(sockfd, buf, sizeof(buf)); //从服务器socket读取数据
        if (read_bytes == -1) {  //发生错误
            close(sockfd);
            printf("socket already disconnected, cannot read any more");
            break;
        } else if (read_bytes == 0) { //通常是服务器关闭了连接
            printf("server socket closed! \n");
            break;
        } else {
            printf("message from server: %s\n", buf); //将读取的数据输出到标准输出
        }
    }
    close(sockfd);
    return 0;
}
