#pragma once
#include <memory>
#include <functional>

class Socket;
class Eventloop;
class Channel;
class Acceptor {
private:
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    std::function<void(int)> newConnectionCallback_;

public:
    explicit Acceptor(Eventloop* loop);
    ~Acceptor();

    void setNewConnectionCallback(std::function<void(int)> callback);
    void AcceptConnection();
    
};