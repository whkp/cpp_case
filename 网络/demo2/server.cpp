#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

//错误处理函数
void errif(bool condition, const char *errmsg) {
    if(condition) {
        perror(errmsg);
        exit(EXIT_FAILURE)
    }
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket error");

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    errif(bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1, "bind error");

    errif(listen(sockfd, 10) == -1, "listen error");

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    bzero(&client_addr, sizeof(client_addr));

    int clnt_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_len);
    errif(clnt_sockfd == -1, "accept error");

    printf("new client fd %d! IP: %s  Port: %d\n", clnt_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    //建立一个socket连接后，就可以使用`<unistd.h>`头文件中`read`和`write`来进行网络接口的数据读写操作了
    while (true)
    {
        char buf[1024]; // 缓冲区
        bzero(&buf, sizeof(buf)); // 清空缓冲区
        ssize_t read_bytes = read(clnt_sockfd, buf, sizeof(buf)); // 从客户端读取数据到缓冲区，返回读取数据大小
        if (read_bytes == -1) {
            close(clnt_sockfd);
            errif(true, "read error");
        } else if (read_bytes == 0) {
            printf("client closed.\n");
            close(clnt_sockfd);
            break;
        } else {
            printf("message from client fd %d: %s\n", clnt_sockfd, buf);
            write(clnt_sockfd, buf, read_bytes); // 将缓冲区数据写回客户端
        }
    }
    
    
    return 0;

}