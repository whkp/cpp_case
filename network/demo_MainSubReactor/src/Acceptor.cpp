#include "Acceptor.h"

#include <fcntl.h>
#include <utility>
#include "Channel.h"
#include "Socket.h"
#include "Buffer.h"

Acceptor::Acceptor(Eventloop *loop) {
    //创建socket对象来监听连接
    socket_ = std::make_unique<Socket>();
    InetAddress *addr = new InetAddress("127.0.0.1", 1234);
    socket_->Bind(addr);
    //socket_->setNonBlocking(); //使用阻塞IO较好
    socket_->Listen();

    //创建channel对象来监听socket对象的fd,并设置读回调函数
    channel_ = std::make_unique<Channel>(loop, socket_->getFd());
    channel_->setReadCallback(std::bind(&Acceptor::AcceptConnection, this));
    channel_->enableReading();
    delete addr;
}

Acceptor::~Acceptor() {}

void Acceptor::setNewConnectionCallback(std::function<void(int)> callback) {
    newConnectionCallback_ = callback;
}

//用于接受新的客户端连接
void Acceptor::AcceptConnection() {
    InetAddress *clnt_addr = new InetAddress();
    Socket* clnt_socket = new Socket(socket_->Accept(clnt_addr));
    printf("new client fd : %d! connection from IP:%s Port:%d\n",clnt_socket->getFd(), clnt_addr->getIp(), clnt_addr->getPort());
    clnt_socket->setNonBlocking();
    //如果设置了新连接回调函数，则调用
    if(newConnectionCallback_) {
        newConnectionCallback_(clnt_socket->getFd());
    }
    delete clnt_addr;
}