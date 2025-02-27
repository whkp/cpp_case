#pragma once
#include <functional>

class Epoll;
class Channel;
class Eventloop {
private:
    std::make_unique<Epoll> epoll_;
    bool quit_{false};
public:
    Eventloop();
    ~Eventloop();
    //事件循环来处理事件
    void loop();
    void updateChannel(Channel*);
};