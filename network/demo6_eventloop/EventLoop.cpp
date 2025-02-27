#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include <vector>

EventLoop::EventLoop() : epoll(new Epoll()), quit(false) {
}

EventLoop::~EventLoop() {
    delete epoll;
}

void EventLoop::loop() {
    while(!quit) {
        std::vector<Channel*> chs;
        chs = epoll->poll();
        for(auto it = chs.begin(); it != chs.end(); ++it) {
            (*it)->handleEvent();
        }
    }
}

void Eventloop::updateChannel(Channel* channel) {
    epoll->updateChannel(channel);
}