#pragma once
#include <memory>
#include <functional>

class Acceptor {
private:
    std::unique<Socket> socket_;
    std::unique<Channel> channel_;
    std::function<void(Socket*)> newConnectionCallback_;

public:
    explict Acceptor(EventLoop* loop);
    ~Acceptor();

    void setNewConnectionCallback(std::function<void(Socket*)> callback);
    void AcceptConnection();
    
};