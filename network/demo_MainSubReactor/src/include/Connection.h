#pragma once
#include <functional>

class Eventloop;
class Socket;
class Buffer;
class Channel;

class Connection {
public:
    enum State {
        Invalid = 1, //无效状态
        Handshaking, //握手中
        Connected,   //已连接
        Closed,
        Failed,      //连接失败
    };
private:
    std::unique_ptr<Socket> sock_;
    std::unique_ptr<Channel> channel_;

    std::unique_ptr<Buffer> readBuffer_;
    std::unique_ptr<Buffer> sendBuffer_;
    Eventloop* loop_;
    std::functional<void(int)> delete_connection_callback_;
    std::functional<void(Connection*)> on_connection_callback_;

    State state_;

    //针对非阻塞和阻塞的读写函数
    void readNonBlocking();
    void writeNoneBlocking();
    void readBlocking();
    void writeBlocking();

public:
    Connection(Eventloop* loop, int fd);
    ~Connection();

    void Read();
    void Write();
    void Send(std::string str);

    void setDeleteConnectionCallback(std::functional<void(int)> const &callback);
    void setOnConnectionCallback(std::functional<void(Connection*)> const &callback);
    State getState() const;
    void Close();
    void setSendBuffer(const char* str);
    Buffer *getReadBuffer();
    Buffer *getSendBuffer();
    const char* SendBufferToStr();
    const char* ReadBufferToStr();
    Socket* getSocket();

    void onConnection(std::function<void()> fn);
};