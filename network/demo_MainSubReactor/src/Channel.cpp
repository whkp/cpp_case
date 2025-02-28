#include "Channel.h"
#include <unistd.h>
#include <utility>
#include "Eventloop.h"
#include "Socket.h"
#include "Epoll.h"


Channel::Channel(Eventloop* loop, int fd)
    : loop_(loop),
      fd_(fd),
      listenEvents_(0),
      readyEvents_(0),
      inEpoll_(false) {    
}

Channel::~Channel() {
    if(fd_ != -1) {
        close(fd_);
        fd_ = -1;
    }
}

void Channel::handleEvent() {
    if(readyEvents_ & (EPOLLIN | EPOLLPRI)) {
        //如果就绪事件包含EPOLLIN（可读） | EPOLLPRI（紧急事件可读），则调用读回调函数
        readCallback_();
    }
    if(readyEvents_ & EPOLLOUT) {
        //如果就绪事件包含EPOLLOUT（可写），则调用写回调函数
        writeCallback_();
    }
}

void Channel::enableReading() {
    //将监听事件设置为可读事件
    listenEvents_ |= EPOLLIN | EPOLLPRI;
    //更新eventloop绑定的epoll类对象
    loop_->updateChannel(this);
}

void Channel::UseET() {
    //设置为ET模式
    listenEvents_ |= EPOLLET;
    loop_->updateChannel(this);
}

int Channel::getFd() const {
    return fd_;
}

uint32_t Channel::getListenEvents() const {
    return listenEvents_;
}

uint32_t Channel::getReadyEvents() const {
    return readyEvents_;
}

bool Channel::getInEpoll() const {
    return inEpoll_;
}

void Channel::setInEpoll(bool inEpoll) {
    inEpoll_ = inEpoll;
}

void Channel::setReadCallback(std::function<void()> const &cb) {
    readCallback_ = cb;
}

void Channel::setWriteCallback(std::function<void()> const &cb) {
    writeCallback_ = cb;
}

void Channel::setReadyEvents(uint32_t ev) {
    readyEvents_ = ev;
}