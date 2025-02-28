#pragma once
#include <functional>
#include <memory>

class Epoll;
class Channel;
class Eventloop {
private:
    std::unique_ptr<Epoll> epoll_;
    bool quit_{false};
public:
    Eventloop();
    ~Eventloop();
    //事件循环来处理事件
    void loop();
    void updateChannel(Channel*);
};