#include "client.h"

client::client() : socket_fd(-1) {}

bool client::connect(const std::string& ip, const unsigned short port) {
    if(socket_fd != -1) return false; //已经连接
    ip_ = ip;
    port_ = port;
    //第一步：创建套接字
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
        return false;
    }
    //第二步：连接服务器
    struct sockaddr_in server_addr;  //用于存放协议、端口、IP地址的结构体
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;  //指定协议族，固定填AF_INET
    server_addr.sin_port = htons(port);  //指定端口，htons将主机字节序转换为网络字节序（大端序）
    struct hostent* h;
    if((h = gethostbyname(ip.c_str())) == nullptr) {  //将域名/主机名/字符串格式转换为IP地址
        ::close(socket_fd);
        socket_fd = -1;
        return false;
    }
    memcpy(&server_addr.sin_addr, h->h_addr, h->h_length);  //将IP地址拷贝到server_addr中
    //加::表示系统调用，不加表示调用自己定义的函数
    if(::connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        ::close(socket_fd);
        socket_fd = -1;
        return false;
    }
    return true;
}

bool client::send(const std::string& buffer) { //不用const char*，因为string可以支持c++风格的字符串
    if(socket_fd == -1) return false;
    if((::send(socket_fd, buffer.data(), buffer.size(), 0)) <= 0) return false;
    return true;
}
bool client::send(void *buffer, const size_t len) {
    if(socket_fd == -1) return false;
    if((::send(socket_fd, buffer, len, 0)) <= 0) return false;
    return true;
}

bool client::sendfile(const std::string& filename, const size_t filesize) {
    std::ifstream fin(filename, std::ios::binary);
    if(!fin.is_open()) {
        std::cout << "open file "<< filename << "error \n";
        return false;
    }
    int onread = 0; //每次读取的字节数
    int totalbytes = 0; //总共读取的字节数
    char buffer[4096]; //缓冲区,一次最多读取4096字节
    while(true) {
        memset(buffer, 0, sizeof(buffer));
        //计算本次读取的字节数
        if(filesize - totalbytes > 4096) {
            onread = 4096;
        } else {
            onread = filesize - totalbytes;
        }
        fin.read(buffer, onread);
        if(send(buffer, onread) == false) {
            std::cout << "send file error \n";
            return false;
        }
        totalbytes += onread;
        if(totalbytes == filesize) break;
    }
    return true;
}

bool client::recv(std::string &buffer, const size_t maxlen) {
    if(socket_fd == -1) return false;
    buffer.clear();
    buffer.resize(maxlen);
    //-1表示出错，0表示连接关闭，>0表示成功
    int readn = ::recv(socket_fd, &buffer[0], maxlen, 0); //buffer.data()返回指向buffer的指针const
    if(readn <= 0) {
        buffer.clear();
        return false;
    }
    buffer.resize(readn);
    return true;

}

bool client::close() {
    if(socket_fd == -1) return false;
    ::close(socket_fd);
    socket_fd = -1;
    return true;
}

client::~client() {
    close();
}