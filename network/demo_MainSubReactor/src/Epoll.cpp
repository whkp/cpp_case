#include "Epoll.h"
#include <unistd.h>
#include <cstring>
#include "Channel.h"
#include "util.h"

#define MAX_EVENTS 1024

Epoll::Epoll() {
    epfd_ =  epoll_create1(0);
    ErrorIf(epfd_ == -1, "epoll_create1 error");
    events_ = new epoll_event[MAX_EVENTS];
    memset(events_, 0, sizeof(*events_) * MAX_EVENTS);
}

Epoll::~Epoll() {
    if(epfd_ != -1) {
        close(epfd_);
        epfd_ = -1;
    }
    delete[] events_;
}

std::vector<Channel*> Epoll::Poll(int timeout) {
    std::vector<Channel*> activeChannels;
    //等待事件发生,返回活跃的文件描述符数量
    int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
    ErrorIf(nfds == -1, "epoll_wait error");
    for(int i = 0; i < nfds; i++) {
        //遍历活跃的文件描述符
        //将epoll_event中的data.ptr转换为Channel*类型
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        activeChannels.emplace_back(channel);
    }
    return activeChannels;
}

void Epoll::UpdateChannel(Channel* channel) {
    int fd = channel->getFd();
    struct epoll_event ev{};
    ev.data.ptr = channel;
    ev.events = channel->getListenEvents();
    if(channel->getInEpoll()) {
        //修改文件描述符在epoll中的事件
        ErrorIf(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll_ctl add error");
    } else {
        ErrorIf(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl  modify error");
        channel->setInEpoll(true);
    }
}

void Epoll::RemoveChannel(Channel* channel) {
    int fd = channel->getFd();
    ErrorIf(epoll_ctl(fd, EPOLL_CTL_DEL, fd, nullptr) == -1, "epoll_ctl del error");
    channel->setInEpoll(false);
}