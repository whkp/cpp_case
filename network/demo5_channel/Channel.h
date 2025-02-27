#pragma once 
#include <sys/epoll.h>

class Epoll;

class Channel {
private:
    Epoll* ep;
    int fd;
    uint32_t events;  //关心的事件
    uint32_t revents; //实际发生的事件
    bool inepoll;
public:
    Channel(Epoll* ep, int fd);
    ~Channel();
    void enableReading();

    int getFd() const;
    uint32_t getRevents() const;
    uint32_t getEvents() const;
    bool getInepoll() const;
    void setInepoll();

    void setRevents(uint32_t rev);
};