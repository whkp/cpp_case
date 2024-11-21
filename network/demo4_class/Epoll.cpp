#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1024

Epoll::Epoll() : epfd(-1), events(nullptr) {
    epfd = epoll_create1(0);
    errif(epfd == -1, "epoll_create1 error");
    events = new epoll_event[MAX_EVENTS];
    memset(events, 0, sizeof(epoll_event) * MAX_EVENTS); // 初始化events数组
}

Epoll::~Epoll() {
    if(epfd != -1) {
        close(epfd);
        epfd = -1;
    }
    delete[] events;
}

void Epoll::addfd(int fd, int et) {
    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = et;
    ev.data.fd = fd;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll_ctl error");
}

std::vector<epoll_event> Epoll::poll(int timeout) {
    std::vector<epoll_event> active_events;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll_wait error");
    for(int i = 0; i < nfds; ++i) {
        active_events.push_back(events[i]);
    }
    return active_events;
}