#include "Epoll.h"
#include "util.h"
#include "Channel.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1024

Epoll::Epoll() : epfd(-1), events(nullptr) {
    epfd = epoll_create1(0);
    errif(epfd == -1, "epoll_create1 error");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(*events)*MAX_EVENTS);
}
Epoll::~Epoll() {
    if (epfd != -1) {
        close(epfd);
        epfd = -1;
    }
    
    delete[] events;
}

void Epoll::addFd(int fd, uint32_t events) {
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = events;
    ev.data.fd = fd;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl error");
}

std::vector<Channel*> Epoll::poll(int timeout) {
    std::vector<Channel*> activeChannels;
    int n = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(n == -1, "epoll_wait error"); //  如果获取失败，则输出错误信息
    for (int i = 0; i < n; ++i) {
        Channel* channel = static_cast<Channel*>(events[i].data.ptr);
        channel->setRevents(events[i].events); //  设置Channel的活跃事件
        activeChannels.push_back(channel);
    }
    return activeChannels;
}

void Epoll::updateChannel(Channel* channel) {
    int fd = channel->getFd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = channel->getEvents();
    ev.data.ptr = channel;
    if (channel->getInepoll()) {
        //修改已经在epoll中的fd
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll_ctl error");
        channel->setInepoll();
    } else {
        // 新增fd
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl error");
    }
}