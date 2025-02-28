#include <vector>
#include "Eventloop.h"
#include "Channel.h"
#include "Epoll.h"

Eventloop::Eventloop() {
    epoll_ = std::make_unique<Epoll>();
}

Eventloop::~Eventloop() {}

void Eventloop::loop() {
    while(!quit_) {
        std::vector<Channel*> activeChannels;
        activeChannels = epoll_->Poll();
        for(auto &channel : activeChannels) {
            //处理每个活跃的文件描述符
            channel->handleEvent();
        }
    }
}

void Eventloop::updateChannel(Channel* channel) {
    epoll_->UpdateChannel(channel);
}