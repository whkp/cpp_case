#pragma once
#include <vector>
#include <sys/epoll.h>

class Channel;
class Epoll {
private:
    int epfd_{-1};
    struct epoll_event *events_{nullptr};
public:
    Epoll();
    ~Epoll();
    void UpdateChannel(Channel*);
    void RemoveChannel(Channel*);
    std::vector<Channel*> Poll(int timeout = -1);
};