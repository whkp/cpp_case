#pragma once
#include <functional>

class Socket;
class EventLoop;
class Channel {
private:
    EventLoop* loop_;
    int fd_;
    uint32_t listenEvents_;  //监听事件
    uint32_t readyEvents_; //已经准备好的事件
    bool inEpoll_; //是否在epoll中
    std::function<void()> readCallback_;  //读回调函数
    std::function<void()> writeCallback_;; //写回调函数

public:
    Channel(EventLoop* loop, int fd);
    ~Channel();
    void handleEvent();
    void enableReading();

    int getFd() const;
    uint32_t getListenEvents() const;
    uint32_t getReadyEvents() const;
    bool getInEpoll() const;
    void setInEpoll(bool inEpoll = true);
    void UseET();

    void setReadCallback(std::function<void()> const &cb); //设置读回调函数
    void setReadyEvents(uint32_t events);

    void setWriteCallback(std::function<void()> const &cb); //设置写回调函数
};