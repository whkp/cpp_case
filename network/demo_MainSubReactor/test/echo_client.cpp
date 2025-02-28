#include <unistd.h>
#include <iostream>
#include "pine.h"

void OneClient(int msgs, int wait) {
    Socket* sock = new Socket();
    sock->Connect("127.0.0.1", 1234);

    Connection* conn = new Connection(nullptr, sock->getFd());
    sleep(wait);
    int count = 0;
    while (count < msgs) {
        conn->setSendBuffer("client send message\n");
        conn->Write();
        //获取连接状态，如果连接关闭，则退出循环
        if(conn->getState() == Connection::State::Closed) {
            conn->Close();
            break;
        }
        conn->Read();
        std::cout << "msg count" << count++ << conn->getReadBuffer() -> to_str() << std::endl;
    }
    delete conn;
    delete sock;
}

int main(int argc, char* argv[]) {

    /*
    单线程测试
    OneClient(10000, 0);
    return 0;
    */
    int threads = 10;
    int msgs = 10000;
    int wait = 0;
    Threadpool *pool = new Threadpool(threads);
    std::function<void()> func = std::bind(OneClient, msgs, wait);
    for(int i = 0; i < threads; i++) {
        pool->addTask(func);
    }
    delete pool;
    return 0;
}