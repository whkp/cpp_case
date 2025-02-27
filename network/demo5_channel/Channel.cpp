#include "Channel.h"
#include "Epoll.h"

Channel::Channel(Epoll* epoll, int fd) : ep(epoll), fd(fd), events(0), revent(0), inepoll(false) {

}

Channel::~Channel() {
    
}

void Channel::enableReading() {
    events |= EPOLLIN;
    ep->updateChannel(this);
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