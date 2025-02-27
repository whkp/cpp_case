#pragma once
#include <arpa/inet.h> //   

//网络地址累类
class InetAddress {
private:
    struct sockaddr_in addr_; //IPv4地址结构体
public:
    InetAddress();
    InetAddress(const char* ip, uint16_t port);
    ~InetAddress();

    void setInetAddr(sockaddr_in addr);
    sockaddr_in getInetAddr() const;
    char* getIp() const;
    uint16_t getPort() const;
};

//网络套接字类
class Socket {
private:
    int fd_; //套接字描述符
public:
    Socket();
    Socket(int fd);
    ~Socket();

    //服务端接口
    void Bind(InetAddress*);
    void Listen();
    int Accept(InetAddress*);
    //客户端接口
    void Connect(InetAddress*);
    void Connect(const char* ip, uint16_t port);
    //设置为非阻塞
    void setNonBlocking();
    bool isNonBlocking() const;
    int getFd() const;
    void setFd(int fd);
};