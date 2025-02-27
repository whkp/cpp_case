#include <vector>
#include "Eventloop.h"
#include "Channel.h"
#include "Epoll.h"

Eventloop::Eventloop() {
    epoll_ = std::make_unique<Epoll>();
}

Eventloop::~Eventloop() {
    delete epoll_;
}

void Eventloop::loop() {
    while(!quit) {
        std::vector<Channel*> activeChannels;
        activeChannels = epoll_->poll();
        for(auto &channel : activeChannels) {
            //处理每个活跃的文件描述符
            channel->handleEvent();
        }
    }
}

void Eventloop::updateChannel(Channel* channel) {
    epoll_->updateChannel(channel);
}