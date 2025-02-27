#pragma once 
#include <sys/epoll.h>
#include <functional>

class Eventloop;

class Channel {
private:
    Eventloop* loop;
    int fd;
    uint32_t events;  //关心的事件
    uint32_t revents; //实际发生的事件
    bool inepoll;
    std::function<void()> callback;
public:
    Channel(Eventloop* loop, int fd);
    ~Channel();
    void handleEvent();
    void enableReading();

    int getFd() const;
    uint32_t getRevents() const;
    uint32_t getEvents() const;
    bool getInepoll() const;
    void setInepoll();

    void setCallback(std::function<void()> cb);
    void setRevents(uint32_t rev);
};