#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include "Eventloop.h"
#include "Acceptor.h"
#include "Connection.h"
#include "Threadpool.h"

class TcpServer {
private:
    std::unique_ptr<Eventloop> main_reactor_;
    std::unique_ptr<Acceptor> acceptor_;

    std::unordered_map<int, std::unique_ptr<Connection>> connections_;
    std::vector<std::unique_ptr<Eventloop>> sub_reactors_;

    std::unique_ptr<Threadpool> threadpool_;

    //自定义业务逻辑，连接后进行怎么样的处理
    std::function<void(Connection*)> on_connect_;
    //接收数据后进行怎么样的处理
    std::function<void(Connection*)> on_recv_;
public:
    TcpServer();
    ~TcpServer();
    void Start();

    void NewConnection(int fd);
    void DeleteConnection(int fd);

    void OnConnection(std::function<void(Connection*)> const &cb);
    void OnRecv(std::function<void(Connection*)> const &cb);
};