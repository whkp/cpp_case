#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

int main() {
    //服务器段建立TCP套接字，对外提供一个网络通信接口
    //在计算机通信领域，socket 被翻译为“套接字”，它是计算机之间进行通信的一种约定或一种方式。
    // 通过 socket 这种约定，一台计算机可以接收其他计算机的数据，也可以向其他计算机发送数据。

    // 创建一个TCP套接字，第一个参数制定IP地址类型，第二个参数制定数据传输方式，第三个参数制定协议类型
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //将套接字与服务器地址绑定
    struct sockaddr_in serv_addr;
    //清空结构体
    bzero(&serv_addr, sizeof(serv_addr));

    //设置服务器地址、IP和端口
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    //将套接字与服务器地址绑定
    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    //监听客户端连接，第二个参数指定监听队列的长度
    listen(sockfd, 128);

    //接受客户端连接
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof(clnt_addr);
    bzero(&clnt_addr, sizeof(clnt_addr));

    //阻塞等待客户端连接，直到有客户端连接成功
    //要注意`accept`和`bind`的第三个参数有一点区别，对于`bind`只需要传入serv_addr的大小即可，而`accept`需要写入客户端socket长度，所以需要定义一个类型为`socklen_t`的变量，并传入这个变量的地址。
    int clnt_sockfd = accept(sockfd, (sockaddr *)&clnt_addr, &clnt_addr_len);
    printf("new client fd %d! client ip: %s, port: %d\n", clnt_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

    return 0;


}