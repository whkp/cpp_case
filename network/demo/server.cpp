#include "server.h"

//初始化服务端接听的socket
bool server::init_server(const unsigned short port) {
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) return false;
    port_ = port;
    struct  sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;   //协议族，IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   //监听所有地址，操作系统所有IP都可以用于通讯
    server_addr.sin_port = htons(port_);   //端口号
    //绑定服务端IP和端口
    if(bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
       close(listen_fd);
       listen_fd = -1;
       return false;
    }
    //设置socket为监听状态
    if(listen(listen_fd, 10) == -1) {
        close(listen_fd);
        listen_fd = -1;
        return false;
    }
    return true;
}

//受理客户端连接，如果没有已连接的客户端，则阻塞等待
bool server::accept_client() {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    if((client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
        listen_fd = -1;
    }

    client_ip = inet_ntoa(client_addr.sin_addr); //客户端IP从大端序转换成字符串
    return true;
}

const std::string &server::get_client_ip() const{
    return client_ip;
}

//与客户端通讯，发送数据
bool server::send_data(const std::string &buffer) {
    if(client_fd == -1) return false;
    if(send(client_fd, buffer.c_str(), buffer.size(), 0) == -1) {
        return false;
    }
    return true;
}

bool server::send_data(void *buffer, const size_t len) {
    if(client_fd == -1) return false;
    if(send(client_fd, buffer, len, 0) <= 0) {
        return false;
    }
    return true;
}

bool server::recv_data(std::string &buffer, const size_t maxlen) {
    buffer.clear();
    buffer.resize(maxlen);
    int readn = recv(client_fd, &buffer[0], maxlen, 0);
    if(readn == -1) {
       buffer.clear();
       return false;
    }
    return true;
}

bool server::recv_data(void *buffer, const size_t len) {
    if(recv(client_fd, buffer, len, 0) == -1) {
        return false;
    }
    return true;
}

bool server::recv_file(const std::string &file_name, const size_t file_size) {
    std::ofstream fout;
    fout.open(file_name, std::ios::binary);
    if(!fout.is_open()) {
        std::cout << "open file failed" << std::endl;
        return false;
    }
    int totalbytes = 0;  //已接受文件的总字节数
    int onread = 0;  //每次接受的字节数
    char buffer[4096]; //接受文件内容的缓冲区，一次最多接受4096字节
    while(true) {
        //计算本次接收的字节数
        if(file_size - totalbytes > 4096) {
            onread = 4096;
        } else {
            onread = file_size - totalbytes;
        }
        //接受文件内容
        if(recv_data(buffer, onread) == false) {
            return false;
        }
        //接受到的内容写入文件
        fout.write(buffer, onread);

        totalbytes += onread;
        if(totalbytes == file_size) break;
    }
    return true;
}


bool server::close_client() {
    if(client_fd == -1) return false;
    close(client_fd);
    client_fd = -1;
    return true;
}

bool server::close_listen() {
    if(listen_fd == -1) return false;
    close(listen_fd);
    listen_fd = -1;
    return true;
}

server::~server() {
    close_listen();
    close_client();
}

