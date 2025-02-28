#include "Connection.h"

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <utility>

#include "Buffer.h"
#include "Channel.h"
#include "Socket.h"
#include "util.h"

Connection::Connection(Eventloop *loop, int fd) {
    sock_ = std::make_unique<Socket>(fd);
    if(loop_ != nullptr) {
        //如果loop不为空，创建channel对象并进行监听
        channel_ = std::make_unique<Channel>(loop_, fd);
        channel_->enableReading();
        channel_->UseET();
    }
    readBuffer_ = std::make_unique<Buffer>();
    sendBuffer_ = std::make_unique<Buffer>();

    state_ = State::Connected;
}

Connection::~Connection() {}

void Connection::Read() {
    if(state_ != State::Connected) {
        //不在连接状态
        perror("Connection is not connected, can not read!");
        return;
    }
    //根据是否阻塞选择对应的处理函数
    readBuffer_->Clear();
    if(sock_->isNonBlocking()) {
        readNonBlocking();
    } else {
        readBlocking();
    }
}

void Connection::Write() {
    if(state_ != State::Connected) {
        perror("Connection is not connected, can not write!");
        return;
    }
    if(sock_->isNonBlocking()) {
        writeNonBlocking();
    } else {
        writeBlocking();
    }
    sendBuffer_->Clear();
    
}

void Connection::readNonBlocking() {
    int sockfd = sock_->getFd();
    //buf大小无所谓
    char buf[1024];
    while(true) {
        memset(buf, 0, sizeof(buf));
        ssize_t n = read(sockfd, buf, sizeof(buf));
        if(n > 0) {
            //读取到数据，写入缓冲区
            readBuffer_->Append(buf, n);
        } else if(n == 0) {
            //对方关闭连接
            printf("read EOF, client fd %d disconnected\n", sockfd);
            state_ = State::Closed;
            Close();
            break;
        } else if(n == -1 && errno == EINTR) {
            //程序正常中断，继续读取
            continue;
        } else if(n == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            //非阻塞IO,表示数据全部读取完毕
            break;
        } else {
            //其他错误
            printf("Other error on client fd %d\n", sockfd);
            state_ = State::Closed;
            Close();
            break;
        }
    }
}

void Connection::writeNonBlocking() {
    int sockfd = sock_->getFd();
    char buf[sendBuffer_->Size()];
    memcpy(buf, sendBuffer_->to_str(), sendBuffer_->Size());
    int data_size = sendBuffer_->Size();
    int data_written = data_size;
    //循环发送数据，直到数据发送完毕
    while(data_written > 0) {
        ssize_t bytes_write = write(sockfd, buf + data_size - data_written, data_written);
        if(bytes_write > 0) {
            data_written -= bytes_write;
        } else if(bytes_write == -1 && errno == EINTR) {
            //被信号中断，继续写
            printf("continue writing\n");
            continue;
        } else if(bytes_write == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            break;
        } else {
            //其他错误
            printf("Other error on client fd %d\n", sockfd);
            state_ = State::Closed;
            break;
        }
    }
}

void Connection::readBlocking() {
    int sockfd = sock_->getFd();
    unsigned int rev_size = 0;
    socklen_t len = sizeof(rev_size);
    //获取缓冲区大小
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &rev_size, &len);
    char buf[rev_size];
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    if(bytes_read > 0) {
        readBuffer_->Append(buf, bytes_read);
    } else if(bytes_read == 0) {
        //对方关闭连接
        printf("read EOF, client fd %d disconnected\n", sockfd);
        state_ = State::Closed;
    }  else if(bytes_read = -1) {
        printf("other error on blocking client fd %d\n", sockfd);
        state_ = State::Closed;
    }
}

void Connection::writeBlocking() {
    int sockfd = sock_->getFd();
    ssize_t bytes_write = write(sockfd, sendBuffer_->to_str(), sendBuffer_->Size());
    if(bytes_write == -1) {
        printf("other error on blocking client fd %d\n", sockfd);
        state_ = State::Closed;
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> const &callback) {
    delete_connection_callback_ = callback;
}

void Connection::setOnConnectionCallback(std::function<void(Connection *)> const &callback) {
    on_connection_callback_ = callback;
}

void Connection::Close() {
    delete_connection_callback_(sock_->getFd());
}

Connection::State Connection::getState() const {
    return state_;
}

Socket *Connection::getSocket() const {
    return sock_.get(); //返回原始socket指针
}

void Connection::setSendBuffer(const char* str) {
    sendBuffer_->setBuf(str);
}
Buffer *Connection::getReadBuffer() const {
    return readBuffer_.get();
}

Buffer *Connection::getSendBuffer() const {
    return sendBuffer_.get();
}

void Connection::Send(std::string std) {
    setSendBuffer(std.c_str());
    Write(); //调用Write()函数发送数据
}