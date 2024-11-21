#pragma once
#include <sys/epoll.h>
#include <vector>

class Epoll {
private:
    int epfd;
    struct epoll_event *events;
public:
    Epoll();
    ~Epoll();
    void addfd(int fd, int et);
    std::vector<epoll_event> poll(int timeout = -1);
}