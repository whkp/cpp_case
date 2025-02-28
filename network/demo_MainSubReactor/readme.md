# Main Sub Reactor 模式

## 1. 概述

Main Sub Reactor 模式，也被称为 Reactor 模式，是一种用于处理并发 I/O 请求的设计模式。它将 I/O 操作分为两个阶段：主 Reactor 负责监听和分发 I/O 事件，而 Sub Reactor 负责处理具体的 I/O 操作。这种模式可以提高系统的并发处理能力，特别是在处理大量并发连接时。

模式特点：

1. 一般只有一个main Reactor，有很多个sub Reactor
2. 服务器管理一个线程池，每一个sub Reactor由一个线程来负责Connection类上的事件循环，事件执行也在该线程
3. main Reactor只负责Acceptor简历新连接，然后将连接分配给sub Reactor

## 2 模块

### 核心TcpServer类

**服务器内拥有的资源都采用智能指针来管理，在服务器实例被销毁时，这些资源也被销毁。**

Tcpserver会构建main_reactor_，并构造接收器acceptor_与main_reactor绑定；将接收器连接回调函数进行设置；回调函数中会将新的连接绑定在一个sub_reactors_

构造线程池，并构造sub_reactors_，将sub_reactors_中放入线程池大小个。

主程序启动接口：

```cpp
void TcpServer::Start() {
    for(size_t i = 0; i < sub_reactors_.size(); ++i) {
        std::function<void()> sub_loop = std::bind(&Eventloop::loop, sub_reactors_[i].get());
        threadpool_->addTask(sub_loop); // 添加任务到线程池
    }
    main_reactor_->loop();
}
```



```c++
class TcpServer {
private:
    //负责接受连接，然后分发给subReactor
    std::unique_ptr<Eventloop> main_reactor_;
    //连接接收器
    std::unique_ptr<Acceptor> acceptor_;
	//Tcp连接
    std::unordered_map<int, std::unique_ptr<Connection>> connections_;
    //处理事件循环
    std::vector<std::unique_ptr<Eventloop>> sub_reactors_;
	//线程池
    std::unique_ptr<Threadpool> threadpool_;
	//自定义业务逻辑，连接后进行怎么样的处理
    std::function<void(Connection*)> on_connect_;
    //接受数据后进行怎么样的处理
    std::function<void(Connection*)> on_recv_;
public:
    TcpServer();
    ~TcpServer();
    //主循环
    void Start();

    void NewConnection(int fd);
    void DeleteConnection(int fd);

    void OnConnection(std::function<void(Connection*)> const &cb);
    void OnRecv(std::function<void(Connection*)> const &cb);
}
```

### Channel 类

**每个channel类指向epoll_event中的data.ptr，自始自终只负责一个文件描述符，对不同的服务、不同事件类型做出相应的处理。Channel类构建时应绑定Epoll，通过Eventloop类与Epoll构建联系**

```cpp
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
```

channel类中的事件循环loop_成员变量为普通指针，channel实例所在事件循环，channel类并不拥有该事件循环资源，仅仅是访问的一个指针。channel类释放时，不应该释放loop_;

**裸指针表示需访问但不拥有的资源**

```cpp
class Channel {
private:
    Eventloop* loop_;
    int fd_;
    uint32_t listenEvents_;  //监听事件
    uint32_t readyEvents_; //已经准备好的事件
    bool inEpoll_; //是否在epoll中
    std::function<void()> readCallback_;  //读回调函数
    std::function<void()> writeCallback_;; //写回调函数
//......
}
```

### Eventloop类

事件循环核心类。绑定一个epoll类，创建该类时会创建一个epoll类。

```cpp
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
```

事件循环：

针对epoll绑定的channel事件，对每个channel进行对应的处理

```cpp
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
```

