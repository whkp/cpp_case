#include "TcpServer.h"

TcpServer::TcpServer() {
    main_reactor_ = std::make_unique<Eventloop>();
    acceptor_ = std::make_unique<Acceptor>(main_reactor_.get());
    std::function<void(int)> cb = std::bind(&TcpServer::NewConnection, this, std::placeholders::_1);
    acceptor_->setNewConnectionCallback(cb);

    unsigned int size = std::thread::hardware_concurrency();
    threadpool_ = std::make_unique<Threadpool>(size);
    for(size_t i = 0; i < size; ++i) {
        std::unique_ptr<Eventloop> sub = std::make_unique<Eventloop>();
        sub_reactors_.push_back(std::move(sub));
    }
}

TcpServer::~TcpServer() {}

void TcpServer::Start() {
    for(size_t i = 0; i < sub_reactors_.size(); ++i) {
        std::function<void()> sub_loop = std::bind(&Eventloop::loop, sub_reactors_[i].get());
        threadpool_->addTask(sub_loop); // 添加任务到线程池
    }
    main_reactor_->loop();
}

void TcpServer::NewConnection(int fd) {
    ErrorIf(fd == -1, "NewConnection error");
    uint64_t randow = fd % sub_reactors_.size(); // 随机选择一个子Reactor
    std::unique_ptr<Connection> conn = std::make_unique<Connection>(sub_reactors_[randow].get(), fd);
    std::function<void(int)> cb = std::bind(&TcpServer::DeleteConnection, this, std::placeholders::_1);

    conn->setDeleteConnectionCallback(cb);
    conn->setOnConnectionCallback(on_connect_);
    connections_[fd] = std::move(conn);
}

void TcpServer::DeleteConnection(int fd) {
    //根据fd找到对应的connection
    auto it = connections_.find(fd);
    if(it != connections_.end()) {
        connections_.erase(it);
    }
}

void TcpServer::OnConnection(std::function<void(Connection*)> const &cb) {
    on_connect_ = std::move(cb);
}
