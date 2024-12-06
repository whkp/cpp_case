#pragma once
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fstream>


class client {
private:
    int socket_fd;
    std::string ip_; //服务器IP/域名
    unsigned short port_; //服务器端口

public:
    client();
    ~client();
    bool connect(const std::string& ip, const unsigned short port);
    bool send(const std::string& buffer);
    bool send(void *buffer, const size_t len);
    bool sendfile(const std::string& filename, const size_t filesize);
    bool recv(std::string& buffer, const size_t maxlen);
    bool close();
};
