#include "Channel.h"
#include "Epoll.h"

Channel::Channel(Eventloop *lp, int fd) : loop(lp), fd(fd), events(0), revent(0), inepoll(false) {

}

Channel::~Channel() {
    
}

void Channel::handleEvent() {
    callback();
}
void Channel::enableReading() {
    events |= EPOLLIN;
    loop->updateChannel(this);
}

int Channel::getFd() {
    return fd;
}

uint32_t Channel::getEvents() {
    return events;
}

uint32_t Channel::getRevents() {
    return revent;
}

bool getInEpoll() {
    return inepoll;
}

void Channel::setInEpoll() {
    inepoll = true;
}

void Channel::setRevents(uint32_t rev) {
    revent = rev;
}

void Channel::setCallback(std::function<void()> cb) {
    callback = cb;
}