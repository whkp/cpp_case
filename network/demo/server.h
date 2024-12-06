#pragma once
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

class server {
private:
    int listen_fd; //服务端用于监听的套接字，-1表示未初始化
    int client_fd; //服务端用于与客户端通信的套接字，-1表示客户端未连接
    std::string client_ip; //客户端字符串格式IP
    unsigned short port_;  //服务端用于通信的端口号
public:
    server(): listen_fd(-1), client_fd(-1) {}
    bool init_server(const unsigned short port);
    bool accept_client();
    bool send_data(const std::string &buffer);
    bool send_data(void *buffer, const size_t len); 
    bool recv_file(const std::string &file_name, const size_t file_size);
    bool recv_data(std::string &buffer, const size_t maxlen);
    bool recv_data(void *buffer, const size_t maxlen);
    bool close_listen();
    bool close_client();
    const std::string &get_client_ip() const;
    ~server();
};